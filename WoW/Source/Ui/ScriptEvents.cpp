#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/QuestFrame.h"

// STATIC: 0x00C24EB0
const char* g_scriptEvents[NUM_SCRIPT_EVENTS];

// STATIC: 0x00C259FC
static TSGrowableArray<UniqueSignal> s_uniqueSignals;

// VERIFY: bb8249cc auto
// OFFSET: 0x00608880
void ScriptEventsInitialize() {
#define SCRIPT_EVENT_NAME(name, index) g_scriptEvents[SCRIPT_EVENT_##name] = #name;
#define SCRIPT_EVENT_ALIAS_NAME(name, string, index) g_scriptEvents[SCRIPT_EVENT_##name] = string;
    SCRIPT_EVENTS(SCRIPT_EVENT_NAME, SCRIPT_EVENT_ALIAS_NAME)
#undef SCRIPT_EVENT_ALIAS_NAME
#undef SCRIPT_EVENT_NAME
}

// VERIFY: a7fea49f auto
// OFFSET: 0x006081B0
static const WOWGUID* GetCurrentQuestGiverGUID() {
    if (CGQuestInfo::m_npcInform != WOWGUID()) {
        return &CGQuestInfo::m_npcInform;
    }

    return &CGQuestInfo::m_npc;
}

// VERIFY: 2a3e0d9d manual
// OFFSET: 0x0060AAA0
static bool ParseTrailingTokens(const char* token, WOWGUID& guid, CGPlayer_C* player) {
    while (*token) {
        if (*token == '-') {
            token++;
        }

        if (!SStrCmpI(token, "target", 6)) {
            token += 6;

            if (guid == ClntObjMgrGetActivePlayer()) {
                guid = CGGameUI::m_lockedTarget;
            } else {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
                guid = unit ? unit->m_unit->m_unitFieldTarget : WOWGUID();
            }
        } else if (!SStrCmpI(token, "pet", 3) && (guid == ClntObjMgrGetActivePlayer() || CGGameUI::IsPartyMember(guid) || (player && (player->m_player->m_playerFlags & 0x80000)))) {
            token += 3;
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                guid = unit->m_unit->m_unitFieldCharm != WOWGUID() ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
            } else {
                guid = WOWGUID();
            }
        } else {
            guid = WOWGUID();
            return false;
        }
    }

    return true;
}

// VERIFY: e4589699 manual
// OFFSET: 0x0060ABF0
bool Script_GetGUIDFromToken(const char* token, WOWGUID& guid, bool defaultToTarget) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!token || !*token) {
        if (!defaultToTarget) {
            return false;
        }

        guid = CGGameUI::m_lockedTarget;
        return true;
    }

    const char* name = token;
    guid = WOWGUID();

    if (!SStrCmpI(token, "player", 6)) {
        token += 6;
        guid = player ? player->m_obj->m_guid : WOWGUID();
    } else if (!SStrCmpI(token, "vehicle", 7)) {
        token += 7;
        guid = player && player->IsSeatedInVehicle() ? WOWGUID { player->GetTransportGUID() } : WOWGUID();
    } else if (!SStrCmpI(token, "pet", 3)) {
        token += 3;

        if (player) {
            guid = player->m_unit->m_unitFieldCharm != WOWGUID() ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
        } else {
            guid = WOWGUID();
        }
    } else if (!SStrCmpI(token, "target", 6)) {
        token += 6;
        guid = CGGameUI::m_lockedTarget;
    } else if (!SStrCmpI(token, "partypet", 8)) {
        token += 8;
        guid = CGGameUI::GetPartyMemberPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "party", 5)) {
        token += 5;
        guid = CGGameUI::GetPartyMember(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "raidpet", 7)) {
        token += 7;
        guid = CGGameUI::GetRaidMemberPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "raid", 4)) {
        token += 4;
        guid = GetRaidMemberGUID(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "boss", 4)) {
        token += 4;
        guid = CGInstanceEncounter_C::GetBoss(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "arenapet", 8)) {
        token += 8;
        guid = CGBattlefieldInfo::GetArenaOpponentPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "arena", 5)) {
        token += 5;
        guid = CGBattlefieldInfo::GetArenaOpponent(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "commentator", 11)) {
        token += 11;
        guid = CGCommentator::s_Commentator.GetArenaMember(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "mouseover", 9)) {
        token += 9;
        guid = CGGameUI::m_currentObjectTrack;

        if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__) && !CGGameUI::IsRaidMemberOrPet(guid)) {
            guid = WOWGUID();
        }
    } else if (!SStrCmpI(token, "focus", 5)) {
        token += 5;
        guid = CGGameUI::unk_BD07D0;
    } else if (!SStrCmpI(token, "npc", STORM_MAX_STR)) {
        token += 3;
        guid = CGGameUI::m_interactTarget;
    } else if (!SStrCmpI(token, "questnpc", STORM_MAX_STR)) {
        token += 8;
        guid = *GetCurrentQuestGiverGUID();
    } else if (!SStrCmpI(token, "none", STORM_MAX_STR)) {
        token += 4;
        guid.m_guid = 0xFFFFFFFFFFFFFFFFULL;
    }

    if ((guid != WOWGUID() || !*token) && ParseTrailingTokens(token, guid, player)) {
        if (guid == WOWGUID()) {
            guid.m_guid = 0xFFFFFFFFFFFFFFFEULL;
        }

        return true;
    }

    if (guid != WOWGUID() || !Script_GetGUIDFromString(&name, guid) || !ParseTrailingTokens(name, guid, player)) {
        return false;
    }

    if (guid == WOWGUID()) {
        guid.m_guid = 0xFFFFFFFFFFFFFFFEULL;
    }

    return true;
}

// VERIFY: 07aadd67 manual
// OFFSET: 0x0060C1F0
CGUnit_C* GetUnitFromName(const char* token) {
    WOWGUID guid;

    if (!Script_GetGUIDFromToken(token, guid, false)) {
        return nullptr;
    }

    return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
}

// VERIFY: 99bf3109 manual
// OFFSET: 0x006143F0
void Script_SendUniqueSignal(const WOWGUID& guid, int32_t signal) {
    for (uint32_t i = 0; i < s_uniqueSignals.Count(); i++) {
        UniqueSignal& entry = s_uniqueSignals[i];

        if (entry.m_guid == guid && entry.m_signal == signal) {
            return;
        }
    }

    UniqueSignal* entry = s_uniqueSignals.New();
    entry->m_guid = guid;
    entry->m_signal = signal;
}

// VERIFY: 52e11eaa manual
// OFFSET: 0x006144D0
void Script_SendUniqueSignalToAllUnits(int32_t signal) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guid = player->m_obj->m_guid;
        Script_SendUniqueSignal(guid, signal);

        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet) {
            const WOWGUID& target = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
            Script_SendUniqueSignal(target, signal);
        }
    }

    for (uint32_t i = 0; i < CGPartyInfo::GetNumPartyMembers(); i++) {
        if (CGGameUI::GetPartyMember(i)) {
            WOWGUID member = CGGameUI::GetPartyMember(i);
            Script_SendUniqueSignal(member, signal);

            if (CGGameUI::GetPartyMemberPet(i)) {
                WOWGUID pet = CGGameUI::GetPartyMemberPet(i);
                Script_SendUniqueSignal(pet, signal);
            }
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (GetRaidMemberGUID(i)) {
            WOWGUID member = GetRaidMemberGUID(i);
            Script_SendUniqueSignal(member, signal);

            if (CGGameUI::GetRaidMemberPet(i)) {
                WOWGUID pet = CGGameUI::GetRaidMemberPet(i);
                Script_SendUniqueSignal(pet, signal);
            }
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        if (i >= 5) {
            continue;
        }

        if (CGBattlefieldInfo::m_arenaOpponents[i]) {
            WOWGUID opponent = CGBattlefieldInfo::m_arenaOpponents[i];
            Script_SendUniqueSignal(opponent, signal);
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[i], TYPEMASK_UNIT, __FILE__, __LINE__));
        WOWGUID pet;

        if (unit) {
            const WOWGUID& unitPet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
            pet = unitPet;
        } else {
            pet = CGBattlefieldInfo::m_arenaOpponentPets[i][0];
        }

        if (pet) {
            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[i], TYPEMASK_UNIT, __FILE__, __LINE__));
            WOWGUID petGuid;

            if (unit) {
                const WOWGUID& unitPet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
                petGuid = unitPet;
            } else {
                petGuid = CGBattlefieldInfo::m_arenaOpponentPets[i][0];
            }

            Script_SendUniqueSignal(petGuid, signal);
        }
    }

    if (CGGameUI::m_lockedTarget) {
        Script_SendUniqueSignal(CGGameUI::m_lockedTarget, signal);
    }

    if (CGGameUI::unk_BD07D0) {
        Script_SendUniqueSignal(CGGameUI::unk_BD07D0, signal);
    }

    if (CGGameUI::m_interactTarget) {
        Script_SendUniqueSignal(CGGameUI::m_interactTarget, signal);
    }
}

// VERIFY: a5a39ab1 auto
// OFFSET: 0x00615020
void Script_SendUniqueSignal(int32_t signal) {
    WOWGUID guid = WOWGUID();
    Script_SendUniqueSignal(guid, signal);
}

// VERIFY: 196fa519 auto
// OFFSET: 0x00614760
void Script_SendUniqueSignals() {
    uint32_t count = s_uniqueSignals.Count();

    for (uint32_t i = 0; i < count; i++) {
        UniqueSignal& entry = s_uniqueSignals[i];

        if (entry.m_guid) {
            Script_SendUnitSignal(entry.m_guid, entry.m_signal);
        } else {
            FrameScript_SignalEvent(entry.m_signal, nullptr);
        }
    }

    s_uniqueSignals.m_count = 0;
}


// VERIFY: 8923d29a auto
// OFFSET: 0x0060C240
static int32_t UnitFieldChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, fieldOffset >> 2);
    return 1;
}

// VERIFY: b16fa490 auto
// OFFSET: 0x0060C260
static int32_t PlayerInventoryChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_INVENTORY_CHANGED);
    return 1;
}

// VERIFY: 48606772 auto
// OFFSET: 0x0060C280
static int32_t PlayerQuestLogChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_QUEST_LOG_CHANGED);
    return 1;
}

// VERIFY: b5117e70 manual
// OFFSET: 0x00614190
void ScriptEventsRegisterUnit(CGUnit_C* unit) {
    for (uint32_t i = 0; i < sizeof(unit->m_unitMirrorLists) / sizeof(unit->m_unitMirrorLists[0]); i++) {
        if (!g_scriptEvents[i]) {
            continue;
        }

        uint32_t size;

        switch (i) {
            case 0:
            case 2:
            case 6:
            case 12:
                size = sizeof(WOWGUID);
                break;

            case 125:
            case 132:
                size = 28;
                break;

            default:
                size = sizeof(uint32_t);
                break;
        }

        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_UNIT, i * sizeof(uint32_t), size, UnitFieldChangedHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x21C, 0x98, PlayerInventoryChangedHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x28, 0x1F4, PlayerQuestLogChangedHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }
}

// VERIFY: 66a7e016 manual
// OFFSET: 0x00614300
void ScriptEventsUnregisterUnit(CGUnit_C* unit) {
    for (uint32_t i = 0; i < sizeof(unit->m_unitMirrorLists) / sizeof(unit->m_unitMirrorLists[0]); i++) {
        if (g_scriptEvents[i]) {
            ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_UNIT, i * sizeof(uint32_t), UnitFieldChangedHandler, nullptr);
        }
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x21C, PlayerInventoryChangedHandler, nullptr);
        ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x28, PlayerQuestLogChangedHandler, nullptr);
    }
}

// VERIFY: d3c08ef9 auto
// OFFSET: 0x00608370
void UnitNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGGameUI::UnitNameUpdate(guid);
    }
}
