// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "AlternateAppearances.hpp"
#include "wxl/AppearanceProtocol.hpp"
#include "AppearanceCompat.hpp"
#include "AppearanceSession.hpp"
#include "RetailChoices.hpp"
#include "FdidCompat.hpp"
#include "NativeProtocol.hpp"
#include "wxl/AppearanceCatalogVersion.hpp"
#include "vendor/extensions/wxl-db2/src/schemas/AppearanceStore.hpp"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/CRandom.h"
#include <memory>
#include <map>
#include <string>
#include <tuple>

namespace wxl::native::alternate_appearances {
namespace {
namespace ap=wxl::appearance;
namespace db=wxl::runtime::db2::appearance;
struct ComponentOwner {
    CCharacterComponent* value=nullptr;
    ~ComponentOwner(){if(value){if(value->m_data.m_model&&value->m_data.m_model->m_attachParent)value->m_data.m_model->DetachFromParent();CCharacterComponent::FreeComponent(value);}}
};
struct Owner {
    CCharacterComponent* primary=nullptr;
    uint64_t primarySerial=0,session=0,generation=0,guid=0;
    uint32_t primaryHandle=0,race=0,sex=0,classId=0;
    ComponentOwner alternate;
    bool showingAlternate=false;
    uint64_t restoredGuid=0;
    uint32_t restoredRevision=0;
};
std::unique_ptr<Owner> owner;
uint64_t nextGeneration=0;
struct Retained {std::vector<RetailChoicePair> rows;bool blocked=false;};
using RetainKey=std::tuple<uint64_t,uint64_t,uint32_t,uint32_t>;
std::map<RetainKey,Retained> retained;
uint64_t retainedSession=0,retentionFailedSession=0;
struct Attempt {uint64_t session=0,serial=0,guid=0;uint32_t handle=0,race=0,sex=0,classId=0;
    bool operator==(const Attempt& b)const{return session==b.session&&serial==b.serial&&guid==b.guid&&handle==b.handle&&race==b.race&&sex==b.sex&&classId==b.classId;}};
Attempt lastAttempt;
std::string Key(const char* raw){std::string s=raw?raw:"";for(auto& c:s){if(c=='/')c='\\';if(c>='A'&&c<='Z')c=char(c-'A'+'a');}if(s.ends_with(".mdx"))s.replace(s.size()-4,4,".m2");return s;}
uint64_t Session(){return protocol::SessionGeneration(FrameScript_GetContext());}
uint64_t CurrentGuid(){const auto index=CCharacterCreation::m_existingCharacterIndex;const auto* display=index>=0?CCharacterSelection::GetCharacterDisplay(uint32_t(index)):nullptr;return display?display->m_characterInfo.m_guid.m_guid:0;}
bool Current(){
    auto* current=CCharacterCreation::m_character;
    return owner&&current&&owner->primary==current&&owner->primarySerial==current->m_wxlSerial
        &&current->m_data.m_model&&owner->primaryHandle==current->m_data.m_model->m_handle
        &&owner->session==Session()&&owner->race==current->m_data.m_preferences.m_raceID
        &&owner->sex==current->m_data.m_preferences.m_sexID&&owner->classId==current->m_data.m_preferences.m_classId&&owner->guid==CurrentGuid();
}
bool PrimaryAlive(){auto* current=CCharacterCreation::m_character;return owner&&current==owner->primary&&current&&current->m_wxlSerial==owner->primarySerial&&current->m_data.m_model&&current->m_data.m_model->m_handle==owner->primaryHandle;}
CM2Model* Background(){auto* frame=CCharacterCreation::m_charCustomizeFrame;return frame?frame->m_model:nullptr;}
bool Complete(){return Current()&&owner->alternate.value&&AppearanceReady(*owner->primary)&&AppearanceReady(*owner->alternate.value);}
bool Rows(const CCharacterComponent& component,std::vector<ap::Choice>& output){
    std::vector<RetailChoicePair> pairs;
    if(!CaptureRetailChoicePairs(component,pairs)||pairs.empty()||pairs.size()>ap::MaxChoices)return false;
    std::vector<ap::Choice> next;next.reserve(pairs.size());
    for(const auto& pair:pairs)next.push_back({pair.option,pair.choice});
    output.swap(next);return true;
}
}

void ReleaseCreation() noexcept {
    try {
        if(PrimaryAlive()&&owner->showingAlternate&&Background()){
            if(owner->alternate.value&&owner->alternate.value->m_data.m_model->m_attachParent)owner->alternate.value->m_data.m_model->DetachFromParent();
            owner->primary->m_data.m_model->AttachToParent(Background(),0,nullptr,0);
        }
        if(owner&&owner->alternate.value&&owner->session==Session()){
            Retained selection;
            selection.blocked=!CaptureRetailChoicePairs(*owner->alternate.value,selection.rows);
            const RetainKey key{owner->session,owner->guid,owner->race,owner->sex};
            if(retained.count(key)||retained.size()<64)retained[key]=std::move(selection);
            else retentionFailedSession=owner->session;
        }
        // The old primary may already have been retired. Never dereference
        // the borrowed identity unless it is still the actual current owner.
        owner.reset();lastAttempt={};
    }catch(...){owner.reset();lastAttempt={};}
}

void UpdateCreation() noexcept {
    try {
        if(owner&&!Current())ReleaseCreation();
        auto* primary=CCharacterCreation::m_character;
        if(!primary||!primary->m_data.m_model||!primary->m_wxlSerial||!Session())return;
        const auto& preferences=primary->m_data.m_preferences;
        if(retainedSession!=Session()){retained.clear();retainedSession=Session();retentionFailedSession=0;lastAttempt={};}
        if(preferences.m_raceID>255||preferences.m_sexID>1)return;
        const auto alternateRace=ap::AlternateRetailRace(uint8_t(preferences.m_raceID));
        if(!alternateRace)return;
        if(owner){
            owner->alternate.value->RenderPrep(1);
            return;
        }
        if(!AppearanceReady(*primary)||RetailRecipeRace(*primary)!=RetailRaceId(preferences.m_raceID)||!Background())return;
        if(nextGeneration==UINT64_MAX)return;
        const Attempt attempt{Session(),primary->m_wxlSerial,CurrentGuid(),primary->m_data.m_model->m_handle,preferences.m_raceID,preferences.m_sexID,preferences.m_classId};
        if(attempt==lastAttempt)return;lastAttempt=attempt;
        if(retentionFailedSession==Session())return;
        const RetainKey retainedKey{Session(),CurrentGuid(),preferences.m_raceID,preferences.m_sexID};
        const auto oldSelection=retained.find(retainedKey);
        if(oldSelection!=retained.end()&&oldSelection->second.blocked)return;
        db::Recipe recipe;
        if(!db::BuildForCharacter(alternateRace,preferences.m_sexID,nullptr,0,recipe))return;
        const char* path=ResolveModel(recipe.modelFileDataId);
        if(!path)return;
        auto next=std::make_unique<Owner>();
        next->primary=primary;next->primarySerial=primary->m_wxlSerial;next->primaryHandle=primary->m_data.m_model->m_handle;
        next->session=Session();next->generation=++nextGeneration;
        next->guid=CurrentGuid();
        next->race=preferences.m_raceID;next->sex=preferences.m_sexID;next->classId=preferences.m_classId;
        next->alternate.value=CCharacterComponent::AllocComponent();
        if(!next->alternate.value)return;
        ComponentData data;
        // Independent first-use defaults, never copied primary choices.
        data.m_preferences={};data.m_preferences.m_raceID=preferences.m_raceID;
        data.m_preferences.m_sexID=preferences.m_sexID;data.m_preferences.m_classId=preferences.m_classId;
        data.m_flags=2;
        data.m_model=primary->m_data.m_model->m_scene->CreateModel(path,0);
        if(!data.m_model)return;
        if(!data.m_model->IsLoaded(1,0)||Key(data.m_model->m_shared->m_fileName)!=Key(path)
            ||!RetailChoicesReady(data.m_preferences,data.m_model)){
            data.m_model->Release();data.m_model=nullptr;return;
        }
        data.m_model->m_lightingCallback=primary->m_data.m_model->m_lightingCallback;
        data.m_model->m_lightingArg=primary->m_data.m_model->m_lightingArg;
        data.m_model->SetBoneSequence(0xffffffffu,0,0,0,1.0f,1,1);
        if(!next->alternate.value->Init(&data,"WXL alternate creation"))return;
        // Restore the actual previously captured option IDs transactionally.
        // A changed family/schema rejects restoration instead of publishing defaults.
        if(oldSelection!=retained.end()&&!ApplyRetailChoices(*next->alternate.value,oldSelection->second.rows.data(),uint32_t(oldSelection->second.rows.size())))return;
        // Body garment textures/geosets retain native priorities. Do not infer
        // alternate-family helm/shoulder/cape/weapon model paths from wire race.
        for(uint32_t slot=ITEMSLOT_SHIRT;slot<=ITEMSLOT_TABARD;++slot)
            if(primary->m_itemDisplayID[slot])next->alternate.value->AddItem(static_cast<ITEM_SLOT>(slot),primary->m_itemDisplayID[slot],0);
        next->alternate.value->RenderPrep(1);
        if(!AppearanceReady(*next->alternate.value)||RetailRecipeRace(*next->alternate.value)!=alternateRace)return;
        next->alternate.value->m_data.m_model->SetAnimating(0);
        owner=std::move(next);
        SetFacing(CCharacterCreation::m_charFacing);
    }catch(...){}
}

CCharacterComponent* Editor() noexcept {return Current()&&owner->showingAlternate?owner->alternate.value:CCharacterCreation::m_character;}
bool Available() noexcept {try{return Complete();}catch(...){return false;}}
bool IsAlternate() noexcept {return Current()&&owner->showingAlternate;}
bool ShowAlternate(bool alternate) noexcept {
    try {
        if(!Complete()||!Background())return false;
        if(owner->showingAlternate==alternate)return true;
        if(nextGeneration==UINT64_MAX)return false;
        auto* hide=alternate?owner->primary:owner->alternate.value;
        auto* show=alternate?owner->alternate.value:owner->primary;
        if(hide->m_data.m_model->m_attachParent)hide->m_data.m_model->DetachFromParent();
        hide->m_data.m_model->SetAnimating(0);
        show->m_data.m_model->AttachToParent(Background(),0,nullptr,0);
        if(show->m_data.m_model->m_attachParent!=Background()){
            hide->m_data.m_model->AttachToParent(Background(),0,nullptr,0);return false;
        }
        owner->showingAlternate=alternate;
        owner->generation=++nextGeneration;
        appearance_session::InvalidateContext();
        return true;
    }catch(...){return false;}
}
void SetFacing(float facing) noexcept {
    if(Current()&&owner->alternate.value&&owner->alternate.value->m_data.m_model)
        owner->alternate.value->m_data.m_model->SetWorldTransform(C3Vector(0,0,0),facing,1.0f);
}
bool RandomizeEditor() noexcept {
    try {
        if(!IsAlternate())return false;
        auto* component=Editor();
        std::vector<RetailChoicePair> pairs;
        if(!CaptureRetailChoicePairs(*component,pairs))return false;
        for(auto& pair:pairs){const auto n=db::ChoiceCount(pair.option);WXL_ChrChoice choice{};
            if(!n||n>8192||!db::ChoiceAt(pair.option,CRandom::dice_(n,g_rndSeed),choice))return false;
            pair.choice=choice.id;}
        return ApplyRetailChoices(*component,pairs.data(),uint32_t(pairs.size()));
    }catch(...){return false;}
}
bool Capture(ap::Selection& selection,Stamp& stamp) noexcept {
    try {
        if(!Complete())return false;
        const auto& p=owner->primary->m_data.m_preferences;
        if(RetailRecipeRace(*owner->primary)!=RetailRaceId(p.m_raceID)
            ||RetailRecipeRace(*owner->alternate.value)!=ap::AlternateRetailRace(uint8_t(p.m_raceID)))return false;
        auto* a=owner->alternate.value;
        if(a==owner->primary||a->m_wxlSerial==owner->primarySerial||!a->m_data.m_model
            ||a->m_data.m_model==owner->primary->m_data.m_model)return false;
        ap::Selection next;next.catalog=ap::ClientCatalogId;next.race=uint8_t(p.m_raceID);next.sex=uint8_t(p.m_sexID);
        if(!Rows(*owner->primary,next.choices)||!Rows(*a,next.alternate)||!ap::Canonical(next))return false;
        Stamp identity{owner->generation,owner->primarySerial,a->m_wxlSerial,owner->primary,a,
            owner->primaryHandle,a->m_data.m_model->m_handle,RetailChoiceRevision(*owner->primary),RetailChoiceRevision(*a)};
        selection=std::move(next);stamp=identity;return true;
    }catch(...){return false;}
}
bool Restore(uint64_t guid,uint32_t revision,const ap::Selection& selection) noexcept {
    try {
        if(!guid||!revision||!Current()||!owner->alternate.value||selection.catalog!=ap::ClientCatalogId
            ||selection.race!=owner->race||selection.sex!=owner->sex||selection.choices.empty()||selection.alternate.empty())return false;
        if(owner->restoredGuid==guid&&owner->restoredRevision==revision)return true;
        if(nextGeneration==UINT64_MAX)return false;
        ap::Selection canonical=selection;if(!ap::Canonical(canonical))return false;
        std::vector<RetailChoicePair> primary,alternate;
        for(auto pair:canonical.choices)primary.push_back({pair.option,pair.choice});
        for(auto pair:canonical.alternate)alternate.push_back({pair.option,pair.choice});
        if(!ApplyRetailChoicePair(*owner->primary,primary.data(),uint32_t(primary.size()),*owner->alternate.value,alternate.data(),uint32_t(alternate.size())))return false;
        owner->restoredGuid=guid;owner->restoredRevision=revision;
        owner->generation=++nextGeneration;
        appearance_session::InvalidateContext();return true;
    }catch(...){return false;}
}
namespace {
int LuaAvailable(lua_State* L){lua_pushboolean(L,Available());return 1;}
int LuaIsAlternate(lua_State* L){lua_pushboolean(L,IsAlternate());return 1;}
int LuaShow(lua_State* L){lua_pushboolean(L,ShowAlternate(lua_toboolean(L,1)!=0));return 1;}
}
void InitializeLua(){FrameScript_RegisterFunction("WXL_HasAlternateAppearance",LuaAvailable);FrameScript_RegisterFunction("WXL_IsAlternateAppearance",LuaIsAlternate);FrameScript_RegisterFunction("WXL_ShowAlternateAppearance",LuaShow);}
}
