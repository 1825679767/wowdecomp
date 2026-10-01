#include "LFGInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00ACD644
static const char* s_lfgTypeTokens[7] = {
    "LFG_TYPE_NONE",
    "LFG_TYPE_DUNGEON",
    "LFG_TYPE_RAID",
    "LFG_TYPE_QUEST",
    "LFG_TYPE_ZONE",
    "LFG_TYPE_HEROIC_DUNGEON",
    "LFG_TYPE_RANDOM_DUNGEON",
};

// STATIC: 0x00BEACD0
static char s_rewardItemTexture[260];

// VERIFY: c406e473 auto
// OFFSET: 0x00552E10
static int32_t Script_GetLFGRoles(lua_State* L) {
    if (s_cvLfgSelectedRoles) {
        int32_t roles = s_cvLfgSelectedRoles->m_intValue;
        lua_pushboolean(L, roles & 1);
        lua_pushboolean(L, roles & 2);
        lua_pushboolean(L, roles & 4);
        lua_pushboolean(L, roles & 8);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 4;
}

// VERIFY: 8d22b555 manual
// OFFSET: 0x00558060
static int32_t Script_GetLFGRandomCooldownExpiration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t index = player->GetAuraIndex(71328);

    if (index == -1) {
        return 0;
    }

    lua_pushnumber(L, player->GetAura(index)->m_endTime * 0.001);
    return 1;
}

// VERIFY: 56281bf7 manual
// OFFSET: 0x005580E0
static int32_t Script_GetLFGDeserterExpiration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t index = player->GetAuraIndex(71041);

    if (index == -1) {
        return 0;
    }

    lua_pushnumber(L, player->GetAura(index)->m_endTime * 0.001);
    return 1;
}

// VERIFY: b4e04149 auto
// OFFSET: 0x005548F0
static int32_t Script_GetAvailableRoles(lua_State* L) {
    ChrClassesRec* rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
    uint8_t roles = CGLookingForGroup::GetClassRoles(rec->m_id);
    lua_pushboolean(L, roles & 0x2);
    lua_pushboolean(L, roles & 0x4);
    lua_pushboolean(L, roles & 0x8);
    return 3;
}

// VERIFY: c1cf44e1 auto
// OFFSET: 0x00553D60
static int32_t Script_SetLFGRoles(lua_State* L) {
    int32_t roles = StringToBOOL(L, 1, 0) != 0;

    if (StringToBOOL(L, 2, 0)) {
        roles |= 0x2;
    }

    if (StringToBOOL(L, 3, 0)) {
        roles |= 0x4;
    }

    if (StringToBOOL(L, 4, 0)) {
        roles |= 0x8;
    }

    ChrClassesRec* rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
    s_cvLfgSelectedRoles->Set(roles & CGLookingForGroup::GetClassRoles(rec->m_id), true, false, false, true);
    FrameScript_SignalEvent(SCRIPT_EVENT_LFG_ROLE_UPDATE, nullptr);
    return 0;
}

// VERIFY: 9e5c5761 auto
// OFFSET: 0x00552DD0
static int32_t Script_GetLFGTypes(lua_State* L) {
    lua_checkstack(L, 7);

    for (uint32_t i = 0; i < 7; i++) {
        lua_pushstring(L, FrameScript_GetText(s_lfgTypeTokens[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 7;
}

// VERIFY: cdaad151 manual
// OFFSET: 0x005529A0
static int32_t Script_SearchLFGGetJoinedID(lua_State* L) {
    if (!(CGLookingForGroup::m_searchJoinedID & 0xFFFFFF)) {
        return 0;
    }

    lua_pushnumber(L, CGLookingForGroup::m_searchJoinedID & 0xFFFFFF);
    return 1;
}

// VERIFY: 6853d33c auto
// OFFSET: 0x00553120
static int32_t Script_GetRandomDungeonBestChoice(lua_State* L) {
    if (CGLookingForGroup::m_randomDungeonBestChoice) {
        lua_pushnumber(L, CGLookingForGroup::m_randomDungeonBestChoice & 0xFFFFFF);
        return 1;
    }

    return 0;
}

// VERIFY: e200eb4e manual
// OFFSET: 0x005553B0
static int32_t Script_GetNumRandomDungeons(lua_State* L) {
    lua_pushnumber(L, CGLookingForGroup::m_randomDungeons.Count());
    return 1;
}

// VERIFY: 7cf5aeee auto
// OFFSET: 0x00553170
static int32_t Script_CanPartyLFGBackfill(lua_State* L) {
    if (CGPartyInfo::m_realNumMembers && CGPartyInfo::m_realNumMembers < 4 && CGPartyInfo::m_lfgSlot && !(CGPartyInfo::unk_BD1989 & 0x2)) {
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// VERIFY: 5d749f9e auto
// OFFSET: 0x005556D0
static int32_t Script_GetPartyLFGBackfillInfo(lua_State* L) {
    if (!CGPartyInfo::m_realNumMembers || CGPartyInfo::m_realNumMembers >= 4 || !CGPartyInfo::m_lfgSlot || (CGPartyInfo::unk_BD1989 & 0x2)) {
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGPartyInfo::m_lfgSlot & 0xFFFFFF);
    lua_pushstring(L, rec->m_name);
    lua_pushnumber(L, rec->m_id);
    lua_pushnumber(L, rec->m_typeID);
    return 3;
}

// VERIFY: 7827dd34 manual
// OFFSET: 0x00553BA0
static int32_t Script_SearchLFGGetNumResults(lua_State* L) {
    double numPlayers;

    if (CGLookingForGroup::m_searchResults.unk_0000) {
        lua_pushnumber(L, CGLookingForGroup::m_searchResults.unk_007C + CGLookingForGroup::m_searchResults.unk_006C);
        numPlayers = CGLookingForGroup::m_searchResults.unk_000C + CGLookingForGroup::m_searchResults.unk_0008;
    } else {
        lua_pushnumber(L, 0.0);
        numPlayers = 0.0;
    }

    lua_pushnumber(L, numPlayers);
    return 2;
}

// VERIFY: c0039394 manual
// OFFSET: 0x00554BE0
static int32_t Script_GetLFDChoiceOrder(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceOrder([table])");
        return 0;
    }

    if (lua_type(L, 1) == LUA_TTABLE) {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    } else {
        lua_createtable(L, 0, 0);
    }

    uint32_t count = CGLookingForGroup::m_lfdEntries.Count();
    uint32_t key = 1;

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfdEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfdEntries.m_data[i];
            lua_pushnumber(L, key++);
            lua_pushnumber(L, entry.m_id);
            lua_settable(L, -3);
        }
    }

    return 1;
}

// VERIFY: 49aafc6d manual
// OFFSET: 0x00554D20
static int32_t Script_GetLFRChoiceOrder(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFRChoiceOrder([table])");
        return 0;
    }

    if (lua_type(L, 1) == LUA_TTABLE) {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    } else {
        lua_createtable(L, 0, 0);
    }

    uint32_t count = CGLookingForGroup::m_lfrEntries.Count();
    uint32_t key = 1;

    for (uint32_t i = 0; i < count; i++) {
        if (i < CGLookingForGroup::m_lfrEntries.Count()) {
            CGLookingForGroup::LFGEntryInfo entry = CGLookingForGroup::m_lfrEntries.m_data[i];
            lua_pushnumber(L, key++);
            lua_pushnumber(L, entry.m_id);
            lua_settable(L, -3);
        }
    }

    return 1;
}

// VERIFY: 66873f4c auto
// OFFSET: 0x00552E90
static int32_t Script_GetLFGInfoLocal(lua_State* L) {
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.m_joined);
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.unk_0004);
    lua_pushboolean(L, CGLookingForGroup::m_localInfo.unk_0005);
    lua_pushstring(L, CGLookingForGroup::m_localInfo.m_comment);
    lua_pushnumber(L, CGLookingForGroup::m_localInfo.m_numDungeons);

    for (uint32_t i = 0; i < 3; i++) {
        lua_pushnumber(L, CGLookingForGroup::m_localInfo.m_roles[i]);
    }

    return 8;
}

// VERIFY: e6f7d5e1 auto
// OFFSET: 0x00552F30
static int32_t Script_GetLFGInfoServer(lua_State* L) {
    LFGInfo* info;
    int32_t inParty;

    if (CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) {
        info = &CGLookingForGroup::m_partyInfo;
        inParty = 1;
    } else {
        info = &CGLookingForGroup::m_playerInfo;
        inParty = 0;
    }

    lua_pushboolean(L, inParty);
    lua_pushboolean(L, info->m_joined);
    lua_pushboolean(L, info->m_queued);
    lua_pushboolean(L, info->unk_0004);
    lua_pushboolean(L, info->unk_0005);
    lua_pushstring(L, info->m_comment);
    lua_pushnumber(L, info->m_numDungeons);

    for (uint32_t i = 0; i < 3; i++) {
        lua_pushnumber(L, info->m_roles[i]);
    }

    return 10;
}

// VERIFY: 1276fa18 manual
// OFFSET: 0x00553F90
static int32_t Script_GetLFGQueueStats(lua_State* L) {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

    if (!info->m_joined || !info->m_queued || !CGLookingForGroup::m_queueStatus.m_lfgID) {
        return 0;
    }

    lua_pushboolean(L, 1);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_tanksNeeded);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_healersNeeded);
    lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_damageNeeded);
    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_queueStatus.m_lfgID);

    if (rec) {
        lua_pushnumber(L, rec->m_typeID);
        lua_pushstring(L, rec->m_name);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_tankWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_healerWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_damageWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_myWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_averageWait);
        lua_pushnumber(L, CGLookingForGroup::m_queueStatus.m_queuedTime * 0.001);
        return 13;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 13;
}

// VERIFY: c766a268 manual
// OFFSET: 0x00557520
static int32_t Script_GetLFGQueuedList(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDQueuedList([table])");
        return 0;
    }

    if (lua_type(L, 1) == LUA_TTABLE) {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    } else {
        lua_createtable(L, 0, 0);
    }

    LFGInfo* info = CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;

    for (LFGDungeonEntry* entry = info->m_dungeons.Head(); entry; entry = info->m_dungeons.Next(entry)) {
        lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
        lua_pushboolean(L, 1);
        lua_settable(L, -3);
    }

    return 1;
}

// VERIFY: 3fc37ebb manual
// OFFSET: 0x00557890
static int32_t Script_GetLFDLockPlayerCount(lua_State* L) {
    if (!CGLookingForGroup::m_playerLockInfo.m_valid) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    if (!CGLookingForGroup::m_partyLockInfo.m_valid) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    int32_t count = 0;

    for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); member; member = CGLookingForGroup::m_partyLockInfo.m_members.Next(member)) {
        count++;
    }

    lua_pushnumber(L, count + 1);
    return 1;
}

// VERIFY: ea869eb8 manual
// OFFSET: 0x00557E40
static int32_t Script_GetLFGCompletionReward(lua_State* L) {
    if (!CGLookingForGroup::m_reward.m_actualSlot) {
        return 0;
    }

    LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_reward.m_actualSlot & 0xFFFFFF);

    if (!rec) {
        return 0;
    }

    lua_pushstring(L, rec->m_name);
    lua_pushnumber(L, rec->m_typeID);
    lua_pushstring(L, rec->m_textureFilename);
    lua_pushnumber(L, CGLookingForGroup::m_reward.m_baseMoney);
    lua_pushnumber(L, CGLookingForGroup::m_reward.m_variableMoney);
    lua_pushnumber(L, CGLookingForGroup::m_reward.m_baseXP);
    lua_pushnumber(L, CGLookingForGroup::m_reward.m_variableXP);
    lua_pushnumber(L, CGLookingForGroup::m_reward.m_strangers);
    int32_t count = 0;

    for (LFGRewardItem* item = CGLookingForGroup::m_reward.m_items.Head(); item; item = CGLookingForGroup::m_reward.m_items.Next(item)) {
        count++;
    }

    lua_pushnumber(L, count);
    return 9;
}

// VERIFY: 4b5bb6fd manual
// OFFSET: 0x00557F70
static int32_t Script_GetLFGCompletionRewardItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLFGCompletionRewardItem(index)");
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    LFGRewardItem* item = CGLookingForGroup::m_reward.m_items.Head();

    while (true) {
        if (!item) {
            return 0;
        }

        if (!index--) {
            break;
        }

        item = CGLookingForGroup::m_reward.m_items.Next(item);
    }

    const char* path = ClientDBStringLookup(3);
    SStrPrintf(s_rewardItemTexture, sizeof(s_rewardItemTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayID));
    lua_pushstring(L, s_rewardItemTexture);
    lua_pushnumber(L, item->m_quantity);
    return 2;
}

// VERIFY: dcf03527 manual
// OFFSET: 0x005576B0
static int32_t Script_GetLFDChoiceLockedState(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE && lua_type(L, 1) > LUA_TNIL) {
        luaL_error(L, "Usage: GetLFDChoiceLockedState([table])");
        return 0;
    }

    if (lua_type(L, 1) == LUA_TTABLE) {
        luaL_checkstack(L, 3, "");
        lua_pushnil(L);

        while (lua_next(L, -2)) {
            lua_settop(L, -2);
            lua_pushvalue(L, -1);
            lua_pushnil(L);
            lua_rawset(L, 1);
        }

        lua_settop(L, 1);
    } else {
        lua_createtable(L, 0, 0);
    }

    if (CGLookingForGroup::m_partyLockInfo.m_valid) {
        for (LFGLockInfo* member = CGLookingForGroup::m_partyLockInfo.m_members.Head(); member; member = CGLookingForGroup::m_partyLockInfo.m_members.Next(member)) {
            for (LFGDungeonEntry* entry = member->m_dungeons.Head(); entry; entry = member->m_dungeons.Next(entry)) {
                if ((entry->m_slot >> 24) != 2 && entry->m_locked) {
                    lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
                    lua_pushnumber(L, entry->m_lockReason);
                    lua_settable(L, -3);
                }
            }
        }
    }

    if (CGLookingForGroup::m_playerLockInfo.m_valid) {
        for (LFGDungeonEntry* entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Head(); entry; entry = CGLookingForGroup::m_playerLockInfo.m_dungeons.Next(entry)) {
            if (entry->m_locked) {
                lua_pushnumber(L, entry->m_slot & 0xFFFFFF);
                lua_pushnumber(L, entry->m_lockReason);
                lua_settable(L, -3);
            }
        }
    }

    return 1;
}

// VERIFY: c82601c8 auto
// OFFSET: 0x005586D0
static int32_t Script_IsListedInLFR(lua_State* L) {
    LFGInfo* info = CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers ? &CGLookingForGroup::m_partyInfo : &CGLookingForGroup::m_playerInfo;
    lua_pushboolean(L, CGLookingForGroup::HasRaidDungeon(info));
    return 1;
}

// VERIFY: 31a496e4 auto
// OFFSET: 0x00554680
static int32_t Script_GetLFGRoleUpdate(lua_State* L) {
    return CGLookingForGroup::GetLFGRoleUpdate(L);
}

// VERIFY: bd40434c auto
// OFFSET: 0x00554740
static int32_t Script_GetLFGRoleUpdateSlot(lua_State* L) {
    return CGLookingForGroup::GetLFGRoleUpdateSlot(L);
}

// VERIFY: 1da53d3d manual
// OFFSET: 0x00554170
static int32_t PushLFGProposalInfo(lua_State* L) {
    if (!CGLookingForGroup::m_proposal.m_id) {
        return 0;
    }

    LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(CGLookingForGroup::m_proposal.m_id & 0xFFFFFF);

    if (!dungeon) {
        return 0;
    }

    lua_pushboolean(L, 1);
    lua_pushnumber(L, CGLookingForGroup::m_proposal.m_id >> 24);
    lua_pushnumber(L, CGLookingForGroup::m_proposal.m_id & 0xFFFFFF);
    lua_pushstring(L, dungeon->m_name);
    lua_pushstring(L, dungeon->m_textureFilename);

    uint32_t roles = 0;
    uint32_t accepted = 0;

    for (uint32_t i = 0; i < 5; i++) {
        if (CGLookingForGroup::m_proposal.m_members[i].m_isLocal) {
            roles = CGLookingForGroup::m_proposal.m_members[i].m_roles;
            accepted = CGLookingForGroup::m_proposal.m_members[i].m_accepted;
            break;
        }
    }

    if (roles & 0x2) {
        lua_pushstring(L, "TANK");
    } else if (roles & 0x4) {
        lua_pushstring(L, "HEALER");
    } else if (roles & 0x8) {
        lua_pushstring(L, "DAMAGER");
    } else {
        lua_pushstring(L, "UNKNOWN");
    }

    lua_pushboolean(L, accepted);

    uint32_t total = 0;
    uint32_t completed = 0;

    if (dungeon->m_typeID == 6) {
        total = 0;
        completed = CGLookingForGroup::m_proposal.m_completedMask != 0;
    } else if (dungeon->m_flags & 0x4) {
        total = 1;
        completed = CGLookingForGroup::m_proposal.m_completedMask != 0;
    } else {
        DungeonEncounterDB::CountMatchingRows(dungeon->m_mapID, dungeon->m_difficulty, CGLookingForGroup::m_proposal.m_completedMask, &total, &completed);
    }

    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    lua_pushnumber(L, 5.0);
    lua_pushboolean(L, roles & 0x1);
    lua_pushboolean(L, (dungeon->m_flags >> 2) & 0x1);
    return 12;
}

// VERIFY: a41527a2 auto
// OFFSET: 0x00554370
static int32_t Script_GetLFGProposal(lua_State* L) {
    return PushLFGProposalInfo(L);
}

// STATIC: 0x00ACD668
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLFGDungeon", &Script_SetLFGDungeon },
    { "ClearLFGDungeon", &Script_ClearLFGDungeon },
    { "ClearAllLFGDungeons", &Script_ClearAllLFGDungeons },
    { "GetLFGInfoLocal", &Script_GetLFGInfoLocal },
    { "GetLFGInfoServer", &Script_GetLFGInfoServer },
    { "GetLFGQueuedList", &Script_GetLFGQueuedList },
    { "SetLFGComment", &Script_SetLFGComment },
    { "LFGTeleport", &Script_LFGTeleport },
    { "JoinLFG", &Script_JoinLFG },
    { "LeaveLFG", &Script_LeaveLFG },
    { "RefreshLFGList", &Script_RefreshLFGList },
    { "SearchLFGJoin", &Script_SearchLFGJoin },
    { "SearchLFGGetJoinedID", &Script_SearchLFGGetJoinedID },
    { "SearchLFGLeave", &Script_SearchLFGLeave },
    { "SearchLFGGetNumResults", &Script_SearchLFGGetNumResults },
    { "SearchLFGGetResults", &Script_SearchLFGGetResults },
    { "SearchLFGGetEncounterResults", &Script_SearchLFGGetEncounterResults },
    { "SearchLFGGetPartyResults", &Script_SearchLFGGetPartyResults },
    { "SearchLFGSort", &Script_SearchLFGSort },
    { "GetLFGTypes", &Script_GetLFGTypes },
    { "GetLFGRoleUpdate", &Script_GetLFGRoleUpdate },
    { "GetLFGRoleUpdateSlot", &Script_GetLFGRoleUpdateSlot },
    { "GetLFGRoleUpdateMember", &Script_GetLFGRoleUpdateMember },
    { "GetLFGRoles", &Script_GetLFGRoles },
    { "SetLFGRoles", &Script_SetLFGRoles },
    { "CompleteLFGRoleCheck", &Script_CompleteLFGRoleCheck },
    { "GetLFGProposal", &Script_GetLFGProposal },
    { "GetLFGProposalMember", &Script_GetLFGProposalMember },
    { "GetLFGProposalEncounter", &Script_GetLFGProposalEncounter },
    { "AcceptProposal", &Script_AcceptProposal },
    { "RejectProposal", &Script_RejectProposal },
    { "GetLFGBootProposal", &Script_GetLFGBootProposal },
    { "SetLFGBootVote", &Script_SetLFGBootVote },
    { "GetLFGQueueStats", &Script_GetLFGQueueStats },
    { "GetLastQueueStatusIndex", &Script_GetLastQueueStatusIndex },
    { "GetLFGDungeonInfo", &Script_GetLFGDungeonInfo },
    { "GetLFGRandomDungeonInfo", &Script_GetLFGRandomDungeonInfo },
    { "IsLFGDungeonJoinable", &Script_IsLFGDungeonJoinable },
    { "SetLFGHeaderCollapsed", &Script_SetLFGHeaderCollapsed },
    { "SetLFGDungeonEnabled", &Script_SetLFGDungeonEnabled },
    { "GetLFGCompletionReward", &Script_GetLFGCompletionReward },
    { "GetLFGCompletionRewardItem", &Script_GetLFGCompletionRewardItem },
    { "IsInLFGDungeon", &Script_IsInLFGDungeon },
    { "CanPartyLFGBackfill", &Script_CanPartyLFGBackfill },
    { "GetPartyLFGBackfillInfo", &Script_GetPartyLFGBackfillInfo },
    { "PartyLFGStartBackfill", &Script_PartyLFGStartBackfill },
    { "GetLFGRandomCooldownExpiration", &Script_GetLFGRandomCooldownExpiration },
    { "UnitHasLFGRandomCooldown", &Script_UnitHasLFGRandomCooldown },
    { "GetLFGDeserterExpiration", &Script_GetLFGDeserterExpiration },
    { "UnitHasLFGDeserter", &Script_UnitHasLFGDeserter },
    { "GetAvailableRoles", &Script_GetAvailableRoles },
    { "GetLFDChoiceOrder", &Script_GetLFDChoiceOrder },
    { "GetLFDChoiceInfo", &Script_GetLFDChoiceInfo },
    { "GetLFDChoiceCollapseState", &Script_GetLFDChoiceCollapseState },
    { "GetLFDChoiceEnabledState", &Script_GetLFDChoiceEnabledState },
    { "GetLFDChoiceLockedState", &Script_GetLFDChoiceLockedState },
    { "GetLFDLockPlayerCount", &Script_GetLFDLockPlayerCount },
    { "GetLFDLockInfo", &Script_GetLFDLockInfo },
    { "RequestLFDPlayerLockInfo", &Script_RequestLFDPlayerLockInfo },
    { "RequestLFDPartyLockInfo", &Script_RequestLFDPartyLockInfo },
    { "GetNumRandomDungeons", &Script_GetNumRandomDungeons },
    { "GetLFGDungeonRewards", &Script_GetLFGDungeonRewards },
    { "GetLFGDungeonRewardInfo", &Script_GetLFGDungeonRewardInfo },
    { "GetLFGDungeonRewardLink", &Script_GetLFGDungeonRewardLink },
    { "GetRandomDungeonBestChoice", &Script_GetRandomDungeonBestChoice },
    { "GetLFRChoiceOrder", &Script_GetLFRChoiceOrder },
    { "IsListedInLFR", &Script_IsListedInLFR },
};

// VERIFY: cc0f9700 auto
// OFFSET: 0x005531D0
void LookingForGroupRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
