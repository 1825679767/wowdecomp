#include "WXL/PrivateAppearances.hpp"
#include "WXL/BarberAppearance.hpp"
// Source-native appearance prepare owner. Wire grammar is shared with server.
// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "AppearanceSession.hpp"
#include "AlternateAppearances.hpp"
#include "NativeProtocol.hpp"
#include "NativeFeatures.hpp"
#include "RetailChoices.hpp"
#include "wxl/AppearanceProtocol.hpp"
#include "wxl/AppearanceCatalogVersion.hpp"
#include "vendor/extensions/wxl-db2/src/schemas/AppearanceStore.hpp"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Model2/M2Model.h"

#include <algorithm>
#include <array>
#include <map>
#include <mutex>
#include <string>

namespace wxl::native::appearance_session {
namespace {
namespace ap = wxl::appearance;
namespace db = wxl::runtime::db2::appearance;
constexpr uint32_t TimeoutMs = 10000;
enum class Phase { None, NeedHello, WaitHello, NeedPrepare, WaitPrepare, Acknowledged, Dispatching, Consumed, Rejected };
struct Context {
    uint64_t session = 0, guid = 0, componentSerial = 0;
    const CCharacterComponent* component = nullptr;
    const CM2Model* root = nullptr;
    uint32_t choiceRevision = 0, classId = 0, customizeFlags = 0, modelHandle = 0;
    int32_t existingIndex = -1;
    std::array<uint32_t, 7> profile{};
    std::string name;
    ap::Selection selection;
    alternate_appearances::Stamp forms;
};
struct Pending {
    Context context;
    Phase phase = Phase::None;
    uint32_t started = 0, request = 0;
    uint8_t status = 255;
    bool permit = false;
};
std::mutex mutex;
Pending pending;
bool installed = false, ready = false, sessionLostNotice = false;
uint64_t readySession = 0, nextRequest = 0;
std::map<uint64_t, ap::Snapshot> snapshots;
struct QueryState { uint32_t attempts = 0, lastSent = 0, lastTouched = 0, received = 0; };
std::map<uint64_t, QueryState> queries;
struct Applied {
    uint64_t session = 0, serial = 0;
    const CCharacterComponent* component = nullptr;
    const CM2Model* root = nullptr;
    uint32_t modelHandle = 0, revision = 0;
};
std::map<uint64_t, Applied> applied;
uint64_t helloSession = 0;
uint32_t helloAttempts = 0, helloSentAt = 0;
bool helloWaiting = false, helloRejected = false;
constexpr uint32_t RetryMs = 3000, MaxAttempts = 3, RefreshMs = 60000;
uint32_t queryWindow = 0, queryCount = 0, queryPruneAt = 0;
void PruneQueries(uint32_t now) {
    if (now-queryPruneAt < 1000) return;
    queryPruneAt=now;
    for (auto it = queries.begin(); it != queries.end();) {
        if (now - it->second.lastTouched > 120000) {
            snapshots.erase(it->first); applied.erase(it->first); it = queries.erase(it);
        } else ++it;
    }
}

void Dialog(const char* text) {
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
}
void Reset(void*) noexcept {
    try { std::lock_guard lock(mutex);
        if (pending.phase != Phase::None && pending.phase != Phase::Consumed) sessionLostNotice = true;
        pending = {}; ready = false; readySession = 0; snapshots.clear(); queries.clear(); applied.clear();
        helloSession = 0; helloAttempts = helloSentAt = 0; helloWaiting = helloRejected = false; queryWindow = queryCount = queryPruneAt = 0;
    } catch (...) {}
}
bool Same(const Context& a, const Context& b) {
    return a.session == b.session && a.guid == b.guid && a.componentSerial == b.componentSerial &&
        a.modelHandle == b.modelHandle && a.component == b.component && a.root == b.root &&
        a.choiceRevision == b.choiceRevision && a.classId == b.classId && a.customizeFlags == b.customizeFlags &&
        a.existingIndex == b.existingIndex && a.profile == b.profile && a.name == b.name &&
        a.selection.catalog == b.selection.catalog && a.selection.race == b.selection.race &&
        a.selection.sex == b.selection.sex && a.selection.choices == b.selection.choices &&
        a.selection.alternate == b.selection.alternate && a.forms == b.forms;
}
bool Build(const char* name, Context& out, bool structural = false) {
    if (!AppearanceSaveEnabled() || !ProtocolEnabled() || (!name && !structural)) return false;
    size_t length = 0;
    if (name) while (length <= 48 && name[length]) ++length;
    if ((!length && !structural) || length > 48) return false;
    const uint64_t session = protocol::SessionGeneration(FrameScript_GetContext());
    auto* c = CCharacterCreation::m_character;
    if (!session || !c || !c->m_wxlSerial || !c->m_data.m_model || (c->m_data.m_flags & 1u) ||
        !RetailChoicesReady(c->m_data.m_preferences, c->m_data.m_model)) return false;
    const auto& p = c->m_data.m_preferences;
    if (p.m_raceID > 255 || p.m_sexID > 1) return false;
    const auto* race = ap::FindRace(uint8_t(p.m_raceID));
    if (!race || !race->creatable || (ap::PrivateModel(uint8_t(p.m_raceID),uint8_t(p.m_sexID))&&!private_appearances::Ready(*c)))return false;
    Context next;
    next.session = session; next.component = c; next.root = c->m_data.m_model;
    next.componentSerial = c->m_wxlSerial; next.modelHandle = c->m_data.m_model->m_handle;
    next.choiceRevision = RetailChoiceRevision(*c);
    next.classId = uint32_t(CCharacterCreation::m_selectedClassID);
    next.existingIndex = CCharacterCreation::m_existingCharacterIndex;
    next.profile = {p.m_raceID, p.m_sexID, p.m_skinID, p.m_faceId, p.m_hairStyleId, p.m_hairColorId, p.m_facialHairId};
    if (length) next.name.assign(name, length);
    next.selection.catalog = ap::ClientCatalogId;
    next.selection.race = uint8_t(p.m_raceID); next.selection.sex = uint8_t(p.m_sexID);
    if (next.existingIndex >= 0) {
        const auto* display = CCharacterSelection::GetCharacterDisplay(uint32_t(next.existingIndex));
        if (!display || !display->m_characterInfo.m_guid) return false;
        next.guid = display->m_characterInfo.m_guid.m_guid;
        next.customizeFlags = display->m_characterInfo.m_customizeFlags;
    }
    if(ap::AlternateRetailRace(uint8_t(p.m_raceID))){
        if(!alternate_appearances::Capture(next.selection,next.forms))return false;
    }else{
        std::vector<RetailChoicePair> pairs;if(!CaptureRetailChoicePairs(*c,pairs)||pairs.empty()||pairs.size()>ap::MaxChoices)return false;
        for(auto pair:pairs)next.selection.choices.push_back({pair.option,pair.choice});
    }
    if (!ap::Canonical(next.selection)||(ap::PrivateModel(next.selection.race,next.selection.sex)&&!private_appearances::Validate(next.selection)))return false;
    out = std::move(next);
    return true;
}
void Receive(uint16_t opcode, const uint8_t* bytes, uint32_t length, uint64_t session, void*) noexcept {
    try {
        if (opcode == ap::SmsgHello) {
            ap::Reader reader(bytes, length); uint8_t version = 0, enabled = 0; uint32_t catalog = 0, raceMap = 0;
            if (!reader.Read(version) || !reader.Read(catalog) || !reader.Read(raceMap) || !reader.Read(enabled) || reader.Remaining()) return;
            std::lock_guard lock(mutex);
            if (!helloWaiting || helloSession != session || !helloAttempts) return;
            ready = version == ap::Version && catalog == ap::ClientCatalogId && raceMap == ap::RaceMapVersion && enabled == 1;
            readySession = ready ? session : 0;
            helloWaiting = false; helloRejected = !ready;
            if ((pending.phase == Phase::NeedHello || pending.phase == Phase::WaitHello) && pending.context.session == session) {
                pending.phase = ready ? Phase::NeedPrepare : Phase::Rejected;
                pending.status = enabled;
            }
        } else if (opcode == ap::SmsgPrepared) {
            ap::Reader reader(bytes, length); uint32_t request = 0; uint8_t status = 255;
            if (!reader.Read(request) || !reader.Read(status) || reader.Remaining()) return;
            std::lock_guard lock(mutex);
            if (pending.phase != Phase::WaitPrepare || pending.context.session != session ||
                !request || request != pending.request || !ready || readySession != session) return;
            pending.status = status;
            pending.phase = status == 0 ? Phase::Acknowledged : Phase::Rejected;
        } else if (opcode == ap::SmsgSnapshot) {
            ap::Snapshot snapshot;
            if (!ap::DecodeSnapshot(bytes, length, snapshot) || snapshot.selection.catalog != ap::ClientCatalogId ||
                (ap::AlternateRetailRace(snapshot.selection.race)&&snapshot.selection.alternate.empty()) ||
                (ap::PrivateModel(snapshot.selection.race,snapshot.selection.sex)&&!private_appearances::Validate(snapshot.selection)))return;
            std::lock_guard lock(mutex);
            if (!ready || readySession != session || !queries.contains(snapshot.guid)) return;
            const auto it = snapshots.find(snapshot.guid);
            if (it != snapshots.end() && it->second.revision > snapshot.revision) return;
            if (it == snapshots.end() && snapshots.size() >= 512) return;
            queries[snapshot.guid].received = OsGetAsyncTimeMs();
            snapshots[snapshot.guid] = std::move(snapshot);
        }
    } catch (...) { Reset(nullptr); }
}
bool WireSend(uint16_t opcode, const std::vector<uint8_t>& bytes) {
    return protocol::Send(opcode, bytes.data(), uint32_t(bytes.size()));
}
enum class HelloResult { Ready, Waiting, Failed };
HelloResult EnsureHello(uint64_t session) {
    if (!session) return HelloResult::Failed;
    const uint32_t now = OsGetAsyncTimeMs();
    {
        std::lock_guard lock(mutex);
        if (ready && readySession == session) return HelloResult::Ready;
        if (helloSession && helloSession != session) return HelloResult::Failed;
        if (helloRejected) return HelloResult::Failed;
        if (helloAttempts && now - helloSentAt < RetryMs) return HelloResult::Waiting;
        if (helloAttempts >= MaxAttempts) return HelloResult::Failed;
        helloSession = session; helloWaiting = true; helloSentAt = now; ++helloAttempts;
    }
    std::vector<uint8_t> wire; ap::Put(wire, ap::Version); ap::Put(wire, ap::ClientCatalogId);
    if (!WireSend(ap::CmsgHello, wire)) {
        std::lock_guard lock(mutex); helloWaiting = false;
    }
    return HelloResult::Waiting;
}
void Fail(const char* message) {
    { std::lock_guard lock(mutex); pending = {}; sessionLostNotice = false; }
    Dialog(message);
}
}

bool Install() noexcept {
    try {
        // The transport calls Install on the FrameScript owner thread.
        if (installed) return true;
        const protocol::NativeDeclaration declarations[] = {
            {ap::CmsgHello, true}, {ap::CmsgPrepare, true}, {ap::CmsgQuery, true},
            {ap::SmsgHello, false, Receive, Reset}, {ap::SmsgSnapshot, false, Receive, Reset},
            {ap::SmsgPrepared, false, Receive, Reset}};
        installed = protocol::RegisterNativeDispatch(declarations,uint32_t(std::size(declarations)));
        return installed;
    } catch (...) { return false; }
}

bool BeforeCreate(const char* name) noexcept {
    try {
        auto* c = CCharacterCreation::m_character;
        if (!c) return false;
        bool dispatching = false;
        { std::lock_guard lock(mutex); dispatching = pending.phase == Phase::Dispatching; }
        if (!dispatching && !RetailSelectionNeedsProtocol(*c)) { Cancel(); return true; }
        if (!installed) { Dialog("Appearance saving is unavailable in this client session."); return false; }
        Context current;
        if (!Build(name, current)) {
            Cancel();
            Dialog("This appearance cannot be saved yet. Check the current model and server connection; both current appearance forms must be ready before saving.");
            return false;
        }
        bool stalePermit = false;
        {
        std::lock_guard lock(mutex);
        if (pending.phase == Phase::Dispatching) {
            if (pending.permit && Same(current, pending.context)) {
                pending.permit = false; pending.phase = Phase::Consumed; return true;
            }
            pending = {}; stalePermit = true;
        }
        if (!stalePermit) {
        // Duplicate UI clicks do not create duplicate prepares or native sends.
        if (pending.phase != Phase::None && Same(current, pending.context)) return false;
        pending = {};
        pending.context = std::move(current);
        pending.started = OsGetAsyncTimeMs();
        pending.phase = ready && readySession == pending.context.session ? Phase::NeedPrepare : Phase::NeedHello;
        sessionLostNotice = false;
        }
        }
        if (stalePermit) Dialog("Appearance saving was cancelled because its prepared character context changed.");
        return false;
    } catch (...) { Cancel(); return false; }
}

bool CanPrepareCurrent() noexcept {
    try { Context current; return installed && Build(nullptr, current, true); }
    catch (...) { return false; }
}

void Tick() noexcept {
    try {
        Pending work;
        bool lost = false;
        { std::lock_guard lock(mutex); lost = sessionLostNotice; sessionLostNotice = false; work = pending; }
        if (lost) { Fail("Appearance saving was cancelled because the server session changed."); return; }
        if (work.phase == Phase::None || work.phase == Phase::Consumed || work.phase == Phase::Dispatching) return;
        if (OsGetAsyncTimeMs() - work.started >= TimeoutMs) { Fail("Appearance saving timed out. No character request was sent; please try again."); return; }
        Context current;
        if (!Build(work.context.name.c_str(), current) || !Same(current, work.context)) {
            Fail("Appearance saving was cancelled because the selected character or appearance changed. Please try again."); return;
        }
        if (work.phase == Phase::Rejected) { Fail("The server rejected the appearance negotiation or prepared selection. Check that client and server use the matching appearance catalog."); return; }
        if (work.phase == Phase::NeedHello || work.phase == Phase::WaitHello) {
            const auto result = EnsureHello(current.session);
            if (result == HelloResult::Failed) { Fail("Appearance negotiation failed or exhausted its bounded retries. Please reconnect before trying again."); return; }
            { std::lock_guard lock(mutex);
                if (!Same(pending.context, current)) return;
                if (pending.phase == Phase::NeedHello || pending.phase == Phase::WaitHello)
                    pending.phase = result == HelloResult::Ready ? Phase::NeedPrepare : Phase::WaitHello;
            }
        } else if (work.phase == Phase::NeedPrepare) {
            uint32_t request = 0;
            { std::lock_guard lock(mutex);
                if (pending.phase != work.phase || !Same(pending.context, current)) return;
                if (nextRequest >= UINT32_MAX) { pending.phase = Phase::Rejected; return; }
                request = uint32_t(++nextRequest); pending.request = request; pending.phase = Phase::WaitPrepare;
            }
            std::vector<uint8_t> wire;
            ap::Put(wire, ap::Version); ap::Put(wire, request); ap::PutSelection(wire, current.selection);
            wire.insert(wire.end(), current.name.begin(), current.name.end()); wire.push_back(0);
            if (!WireSend(ap::CmsgPrepare, wire)) Fail("The appearance selection could not be sent. No character request was sent.");
        } else if (work.phase == Phase::Acknowledged) {
            { std::lock_guard lock(mutex);
                if (pending.phase != Phase::Acknowledged || pending.request != work.request ||
                    pending.status != 0 || !Same(pending.context, current)) return;
                pending.phase = Phase::Dispatching; pending.permit = true;
            }
            // Typed source call, outside every transport/state lock. The native
            // function validates again then consumes this exact one-use permit.
            CCharacterCreation::CreateCharacter(current.name.c_str());
            { std::lock_guard lock(mutex); pending = {}; }
        }
    } catch (...) { Fail("Appearance saving was cancelled before the character request completed."); }
}

void Cancel() noexcept {
    try { std::lock_guard lock(mutex); pending = {}; sessionLostNotice = false; } catch (...) {}
}
void InvalidateContext() noexcept { Cancel(); }

bool QueryCharacter(uint64_t guid) noexcept {
    try {
        const auto session = protocol::SessionGeneration(FrameScript_GetContext());
        if (!guid || !session) return false;
        if (EnsureHello(session) != HelloResult::Ready) return false;
        const uint32_t now = OsGetAsyncTimeMs();
        { std::lock_guard lock(mutex);
            if (!ready || readySession != session) return false;
            if (!queries.contains(guid)) { PruneQueries(now); if (queries.size() >= 512) return false; }
            auto& query = queries[guid]; query.lastTouched = now;
            if (query.received && now-query.received < RefreshMs) return false;
            if (query.received && now-query.received >= RefreshMs && now-query.lastSent >= RefreshMs) query.attempts=0;
            if (now-queryWindow >= 1000) {queryWindow=now;queryCount=0;}
            if (queryCount >= 4) return false;
            if (query.attempts >= MaxAttempts || (query.attempts && now - query.lastSent < RetryMs)) return false;
            ++queryCount; ++query.attempts; query.lastSent = now;
        }
        std::vector<uint8_t> wire; ap::Put(wire, guid);
        if (WireSend(ap::CmsgQuery, wire)) return true;
        return false;
    } catch (...) { return false; }
}
bool QuerySelectedCharacter() noexcept {
    const auto index = CCharacterSelection::s_selectionIndex;
    auto* display = index >= 0 ? CCharacterSelection::GetCharacterDisplay(uint32_t(index)) : nullptr;
    return display && QueryCharacter(display->m_characterInfo.m_guid.m_guid);
}
bool GetSnapshot(uint64_t guid,ap::Snapshot& output) noexcept {try{
 const auto session=protocol::SessionGeneration(FrameScript_GetContext());std::lock_guard lock(mutex);
 if(!session||readySession!=session)return false;const auto it=snapshots.find(guid);if(it==snapshots.end())return false;output=it->second;return true;
}catch(...){return false;}}
bool HasSnapshot(uint64_t guid, uint32_t* revision) noexcept {
    try {
        const auto session = protocol::SessionGeneration(FrameScript_GetContext());
        std::lock_guard lock(mutex);
        if (!session || readySession != session) return false;
        const auto it = snapshots.find(guid); if (it == snapshots.end()) return false;
        if (revision) *revision = it->second.revision;
        return true;
    } catch (...) { return false; }
}

void InvalidateCharacter(uint64_t guid) noexcept {
    try { std::lock_guard lock(mutex); snapshots.erase(guid); queries.erase(guid); applied.erase(guid); }
    catch (...) {}
}

void RefreshCharacter(uint64_t guid, CCharacterComponent* component) noexcept {
    try {
        if(barber_appearance::Editing(guid,component))return;
        if (!guid || !component || !component->m_wxlSerial || component == CCharacterCreation::m_character ||
            !component->m_data.m_model || !AppearanceSaveEnabled() ||
            !RetailChoicesReady(component->m_data.m_preferences, component->m_data.m_model)) return;
        const auto session = protocol::SessionGeneration(FrameScript_GetContext());
        if (!session || EnsureHello(session) != HelloResult::Ready) return;
        ap::Snapshot snapshot;
        bool cached = false, refresh = false;
        {
            std::lock_guard lock(mutex);
            const auto it = snapshots.find(guid);
            if (it != snapshots.end() && readySession == session) {
                snapshot = it->second; cached = true;
                auto& query = queries[guid]; query.lastTouched = OsGetAsyncTimeMs();
                refresh = query.lastTouched - query.received >= RefreshMs;
            }
        }
        if (!cached) { QueryCharacter(guid); return; }
        if (refresh) QueryCharacter(guid);
        const auto& selection = snapshot.selection;
        const auto& preferences = component->m_data.m_preferences;
        if (snapshot.guid != guid || selection.catalog != ap::ClientCatalogId || (ap::PrivateModel(selection.race,selection.sex)&&!private_appearances::Validate(selection)) ||
            selection.race != preferences.m_raceID || selection.sex != preferences.m_sexID ||
            !RetailChoicesReady(preferences, component->m_data.m_model)) return;
        const Applied identity{session, component->m_wxlSerial, component, component->m_data.m_model,
            component->m_data.m_model->m_handle, snapshot.revision};
        {
            std::lock_guard lock(mutex);
            const auto it = applied.find(guid);
            if (it != applied.end()) {
                const auto& old = it->second;
                if (old.session == identity.session && old.serial == identity.serial &&
                    old.component == identity.component && old.root == identity.root &&
                    old.modelHandle == identity.modelHandle && old.revision == identity.revision) return;
            }
        }
        const auto family=RetailRecipeRace(*component);const auto* race=ap::FindRace(selection.race);
        if(!race)return;const auto* rows=(family==race->retail||family==private_appearances::Identity(selection.race,selection.sex))?&selection.choices:family==ap::AlternateRetailRace(selection.race)?&selection.alternate:nullptr;
        if(!rows||rows->empty()||rows->size()>ap::MaxChoices)return;
        std::array<RetailChoicePair, ap::MaxChoices> pairs{};
        for (size_t i = 0; i < rows->size(); ++i)
            pairs[i] = {(*rows)[i].option, (*rows)[i].choice};
        // Transactional native authority validation and component update run
        // outside transport/state locks. No shared model recipe is overwritten.
        const bool success = ApplyRetailChoices(*component, pairs.data(), uint32_t(rows->size()));
        // Failed exact authority applications are memoized as attempts too, so
        // malformed/unavailable snapshots do not run an apply every frame.
        (void)success;
        {
            std::lock_guard lock(mutex);
            if (readySession == session && snapshots.contains(guid) &&
                snapshots.at(guid).revision == snapshot.revision && (applied.contains(guid) || applied.size() < 512))
                applied[guid] = identity;
        }
    } catch (...) {}
}
void RefreshCreationForms() noexcept {try{
 if(!alternate_appearances::Available()||CCharacterCreation::m_existingCharacterIndex<0)return;
 const auto* display=CCharacterSelection::GetCharacterDisplay(uint32_t(CCharacterCreation::m_existingCharacterIndex));if(!display)return;
 const auto guid=display->m_characterInfo.m_guid.m_guid;const auto session=protocol::SessionGeneration(FrameScript_GetContext());if(!guid||!session)return;
 ap::Snapshot snapshot;bool found=false;{std::lock_guard lock(mutex);const auto it=snapshots.find(guid);if(readySession==session&&it!=snapshots.end()){snapshot=it->second;found=true;}}
 if(!found){QueryCharacter(guid);return;}
 alternate_appearances::Restore(guid,snapshot.revision,snapshot.selection);
}catch(...){}}

}
