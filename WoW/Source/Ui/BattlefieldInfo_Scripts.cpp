#include "BattlefieldInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/DB/Rec/WorldStateUIRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"

// VERIFY: 0203d343 manual
// OFFSET: 0x00549E80
static int32_t Script_GetNumBattlefieldScores(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numScores);
    return 1;
}

// VERIFY: b4f9fa30 manual
// OFFSET: 0x00549F20
static int32_t Script_GetNumBattlefieldStats(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numWorldStateStats);
    return 1;
}

// VERIFY: 3b26b6db manual
// OFFSET: 0x0054A140
static int32_t Script_GetNumBattlefieldVehicles(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_numVehicles);
    return 1;
}

// VERIFY: 3368ccc3 auto
// OFFSET: 0x0054A280
static int32_t Script_GetBattlefieldArenaFaction(lua_State* L) {
    if (CGBattlefieldInfo::m_arenaFaction) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9d9b151c manual
// OFFSET: 0x0054A2C0
static int32_t Script_GetNumArenaOpponents(lua_State* L) {
    lua_pushnumber(L, GetArenaPetGUIDCount());
    return 1;
}

// VERIFY: 37f24078 manual
// OFFSET: 0x00549CD0
static int32_t Script_GetBattlefieldInstanceRunTime(lua_State* L) {
    uint32_t start = CGBattlefieldInfo::m_instanceStartTime;

    if (start) {
        lua_pushnumber(L, OsGetAsyncTimeMs() - start);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 80a4f4ea manual
// OFFSET: 0x00549EC0
static int32_t Script_GetBattlefieldWinner(lua_State* L) {
    if (CGBattlefieldInfo::m_mapDone) {
        lua_pushnumber(L, CGBattlefieldInfo::m_winner);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: e8ab005d manual
// OFFSET: 0x0054C7A0
static int32_t Script_HearthAndResurrectFromArea(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_HEARTH_AND_RESURRECT));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: f274cbd6 manual
// OFFSET: 0x00549C40
static int32_t Script_GetBattlefieldInstanceExpiration(lua_State* L) {
    if (CGBattlefieldInfo::m_instanceExpiration) {
        uint32_t now = OsGetAsyncTimeMs();

        if (static_cast<int32_t>(now - CGBattlefieldInfo::m_instanceExpiration) < 0) {
            lua_pushnumber(L, CGBattlefieldInfo::m_instanceExpiration - now);
        } else {
            lua_pushnumber(L, 0.0);
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: 8ae49628 manual
// OFFSET: 0x0054A180
static int32_t Script_GetBattlefieldTeamInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldTeamInfo(index)");
    }

    uint32_t team = static_cast<uint32_t>(lua_tonumber(L, 1));
    lua_pushstring(L, team >= 2 ? nullptr : CGBattlefieldInfo::m_teamNames[team]);
    lua_pushnumber(L, team >= 2 ? 0 : CGBattlefieldInfo::m_teamRatings[team]);
    lua_pushnumber(L, team >= 2 ? 0 : CGBattlefieldInfo::m_newTeamRatings[team]);
    lua_pushnumber(L, team >= 2 ? 0 : CGBattlefieldInfo::m_avgMatchmakingRating[team]);
    return 4;
}

// VERIFY: 978fd379 auto
// OFFSET: 0x00549AD0
static int32_t Script_IsActiveBattlefieldArena(lua_State* L) {
    if (CGBattlefieldInfo::m_instanceType == 4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    uint32_t index = CGBattlefieldInfo::m_activeIndex;
    BattlefieldStatus* status = index < 2 ? &CGBattlefieldInfo::m_status[index] : nullptr;

    if (status && status->m_rated) {
        lua_pushnumber(L, 1.0);
        return 2;
    }

    lua_pushnil(L);
    return 2;
}

// VERIFY: 097b8b8f manual
// OFFSET: 0x00549B80
static int32_t Script_GetBattlefieldPortExpiration(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldPortExpiration(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldStatus* status = index < 2 ? &CGBattlefieldInfo::m_status[index] : nullptr;
    double seconds = 0.0;

    if (status && status->m_portExpiration) {
        uint32_t now = OsGetAsyncTimeMs();

        if (static_cast<int32_t>(now - status->m_portExpiration) < 0) {
            seconds = (status->m_portExpiration - now) / 1000;
        }
    }

    lua_pushnumber(L, seconds);
    return 1;
}

// VERIFY: 256537fc manual
// OFFSET: 0x00549D30
static int32_t Script_GetBattlefieldEstimatedWaitTime(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldEstimatedWaitTime(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldStatus* status = index < 2 ? &CGBattlefieldInfo::m_status[index] : nullptr;
    lua_pushnumber(L, status ? status->m_estimatedWaitTime : 0.0);
    return 1;
}

// VERIFY: 4ad66014 manual
// OFFSET: 0x00549DD0
static int32_t Script_GetBattlefieldTimeWaited(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldTimeWaited(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldStatus* status = index < 2 ? &CGBattlefieldInfo::m_status[index] : nullptr;

    if (status && status->m_queueStartTime) {
        lua_pushnumber(L, OsGetAsyncTimeMs() - status->m_queueStartTime);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 0e8935b5 manual
// OFFSET: 0x0054C250
static int32_t Script_LeaveBattlefield(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_LEAVE_BATTLEFIELD));
    uint64_t queueData = CGBattlefieldInfo::m_activeIndex < 0 ? 0 : CGBattlefieldInfo::m_status[CGBattlefieldInfo::m_activeIndex].m_queueData;
    msg.Put(queueData);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 92d4bec5 manual
// OFFSET: 0x0054BC30
static int32_t Script_GetBattlefieldStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldStatus(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldStatus* status = index < 2 ? &CGBattlefieldInfo::m_status[index] : nullptr;

    if (!status) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 7;
    }

    switch (status->m_status) {
        case 0:
            lua_pushstring(L, "none");
            break;
        case 1:
            lua_pushstring(L, "queued");
            break;
        case 2:
            lua_pushstring(L, "confirm");
            break;
        case 3:
            lua_pushstring(L, "active");
            break;
        default:
            lua_pushstring(L, "error");
            break;
    }

    MapRec* map = nullptr;

    if (status->m_status == 2) {
        if (status->m_bgTypeID == 32) {
            if (status->m_joinedQueueData) {
                BattlemasterListRec* battlemaster = g_battlemasterListDB.GetRecord(static_cast<uint32_t>(status->m_joinedQueueData >> 16));

                if (battlemaster) {
                    map = g_mapDB.GetRecord(battlemaster->m_mapID[0]);
                }
            }
        } else {
            map = g_mapDB.GetRecord(status->m_mapID);
        }
    } else if (status->m_status == 3) {
        map = g_mapDB.GetRecord(CGBattlefieldInfo::m_activeMapID);
    }

    if (map) {
        lua_pushstring(L, map->m_mapName);
    } else {
        BattlemasterListRec* battlemaster = g_battlemasterListDB.GetRecord(status->m_bgTypeID);

        if (battlemaster) {
            lua_pushstring(L, battlemaster->m_name);
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushnumber(L, status->m_instanceID);
    lua_pushnumber(L, status->m_minLevel);
    lua_pushnumber(L, status->m_maxLevel);
    lua_pushnumber(L, status->m_teamSize);

    if (status->m_rated) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 7;
}

// VERIFY: 1304f61c auto
// OFFSET: 0x0054C740
static int32_t Script_GetBattlefieldMapIconScale(lua_State* L) {
    MapRec* map = g_mapDB.GetRecord(CGBattlefieldInfo::m_activeMapID);

    if (map) {
        lua_pushnumber(L, map->m_minimapIconScale);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: f7e1c741 auto
// OFFSET: 0x0054BAE0
static int32_t Script_IsBattlefieldArena(lua_State* L) {
    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_bgTypeID);

    if (rec && rec->m_instanceType == 4) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 856f59dc auto
// OFFSET: 0x0054C6E0
static int32_t Script_CanJoinBattlefieldAsGroup(lua_State* L) {
    BattlemasterListRec* rec = g_battlemasterListDB.GetRecord(CGBattlefieldInfo::m_bgTypeID);

    if (rec && rec->m_groupsAllowed) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 8263ea2a manual
// OFFSET: 0x0054BAA0
static int32_t Script_GetNumBattlefields(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_instances.Count());
    return 1;
}

// VERIFY: 4f5a19c9 manual
// OFFSET: 0x0054BBD0
static int32_t Script_GetSelectedBattlefield(lua_State* L) {
    uint32_t index = 0;

    for (; index < CGBattlefieldInfo::m_instances.Count(); index++) {
        if (CGBattlefieldInfo::m_instances.m_data[index] == CGBattlefieldInfo::m_selectedInstance) {
            break;
        }
    }

    if (index >= CGBattlefieldInfo::m_instances.Count()) {
        index = -1;
    }

    lua_pushnumber(L, index + 1);
    return 1;
}

// VERIFY: d35b98b8 manual
// OFFSET: 0x0054BB40
static int32_t Script_SetSelectedBattlefield(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedBattlefield(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGBattlefieldInfo::m_instances.Count()) {
        CGBattlefieldInfo::m_selectedInstance = 0;
    } else {
        CGBattlefieldInfo::m_selectedInstance = CGBattlefieldInfo::m_instances.m_data[index];
    }

    return 0;
}

// VERIFY: 2a39d302 manual
// OFFSET: 0x0054D8F0
static int32_t Script_GetBattlefieldInstanceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldInfo(index)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGBattlefieldInfo::m_instances.Count()) {
        return 0;
    }

    lua_pushnumber(L, CGBattlefieldInfo::m_instances.m_data[index]);
    return 1;
}

// VERIFY: c0f60ba2 auto
// OFFSET: 0x0054A300
static int32_t Script_GetHolidayBGHonorCurrencyBonuses(lua_State* L) {
    lua_pushboolean(L, CGBattlefieldInfo::m_hasHolidayWinToday);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayWinHonor);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayWinArena);
    lua_pushnumber(L, CGBattlefieldInfo::m_holidayLossHonor);
    lua_pushnumber(L, 0.0);
    return 5;
}

// VERIFY: fd1d91b3 auto
// OFFSET: 0x0054A370
static int32_t Script_GetRandomBGHonorCurrencyBonuses(lua_State* L) {
    lua_pushboolean(L, CGBattlefieldInfo::m_hasRandomWinToday);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomWinHonor);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomWinArena);
    lua_pushnumber(L, CGBattlefieldInfo::m_randomLossHonor);
    lua_pushnumber(L, 0.0);
    return 5;
}

// VERIFY: 862ed545 manual
// OFFSET: 0x0054A040
static int32_t Script_GetNumBattlefieldPositions(lua_State* L) {
    uint32_t count = CGBattlefieldInfo::m_numPlayerPositions;
    int32_t visible = count;

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid = i < CGBattlefieldInfo::m_numPlayerPositions ? CGBattlefieldInfo::m_playerPositionGuids[i] : WOWGUID();

        if (CGPartyInfo::IsMember(guid) || IsGuidInMyRaid(guid)) {
            visible--;
        }
    }

    lua_pushnumber(L, visible);
    return 1;
}

// VERIFY: f466d1f1 auto
// OFFSET: 0x0054A0E0
static int32_t Script_GetNumBattlefieldFlagPositions(lua_State* L) {
    int32_t count;

    if (CGBattlefieldInfo::m_flagCarriers[1]) {
        count = 2;
    } else {
        count = CGBattlefieldInfo::m_flagCarriers[0] != WOWGUID();
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: 15310d8d manual
// OFFSET: 0x00549F60
static int32_t Script_GetBattlefieldStatData(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetBattlefieldStatData(playerIndex, statIndex)");
    }

    uint32_t player = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    BattlefieldScore* score;
    uint32_t stat;
    double value;

    if (player < CGBattlefieldInfo::m_numScoreEntries && (score = CGBattlefieldInfo::m_scores[player]) && (stat = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1) <= 8) {
        value = score->m_stats[stat];
    } else {
        value = 0.0;
    }

    lua_pushnumber(L, value);
    return 1;
}

// VERIFY: a4ccae58 manual
// OFFSET: 0x0054C170
static int32_t Script_GetBattlefieldStatInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBattlefieldStatInfo(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t id = index < CGBattlefieldInfo::m_numWorldStateStats ? CGBattlefieldInfo::m_worldStateStats[index] : 0;
    WorldStateUIRec* rec = g_worldStateUIDB.GetRecord(id);

    if (rec) {
        lua_pushstring(L, rec->m_string);
        lua_pushstring(L, rec->m_icon);
        lua_pushstring(L, rec->m_tooltip);
        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// VERIFY: 59535cfc manual
// OFFSET: 0x0054C8A0
static int32_t Script_GetWorldPVPQueueStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetWorldPVPQueueStatus(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    WorldPvPQueueStatus* queue = index < 1 ? &CGBattlefieldInfo::m_worldPvPQueues[index] : nullptr;

    if (!queue) {
        return 0;
    }

    switch (queue->m_status) {
        case 0:
            lua_pushstring(L, "none");
            break;

        case 1:
            lua_pushstring(L, "queued");
            break;

        case 2:
            lua_pushstring(L, "confirm");
            break;

        case 3:
            lua_pushstring(L, "active");
            break;

        default:
            lua_pushstring(L, "error");
            break;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(queue->m_areaID);

    if (area) {
        lua_pushstring(L, area->m_areaName);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, queue->m_queueID);

    if (queue->m_status == 2) {
        lua_pushnumber(L, queue->m_expireTime - CGWorldStateInfo::GetUITime());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 4;
}

// VERIFY: 397d2a48 manual
// OFFSET: 0x0054C870
static int32_t Script_GetNumBattlegroundTypes(lua_State* L) {
    lua_pushnumber(L, CGBattlefieldInfo::m_battlegroundTypes.Count());
    return 1;
}

// VERIFY: 97f10f1d auto
// OFFSET: 0x0054DCB0
static int32_t Script_RequestBattlefieldPositions(lua_State* L) {
    CGBattlefieldInfo::RequestPlayerPositions();
    return 0;
}

// STATIC: 0x00ACD178
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumBattlefields", &Script_GetNumBattlefields },
    { "GetBattlefieldInfo", &Script_GetBattlefieldInfo },
    { "GetBattlefieldInstanceInfo", &Script_GetBattlefieldInstanceInfo },
    { "IsBattlefieldArena", &Script_IsBattlefieldArena },
    { "IsActiveBattlefieldArena", &Script_IsActiveBattlefieldArena },
    { "JoinBattlefield", &Script_JoinBattlefield },
    { "SetSelectedBattlefield", &Script_SetSelectedBattlefield },
    { "GetSelectedBattlefield", &Script_GetSelectedBattlefield },
    { "AcceptBattlefieldPort", &Script_AcceptBattlefieldPort },
    { "GetBattlefieldStatus", &Script_GetBattlefieldStatus },
    { "GetBattlefieldPortExpiration", &Script_GetBattlefieldPortExpiration },
    { "GetBattlefieldInstanceExpiration", &Script_GetBattlefieldInstanceExpiration },
    { "GetBattlefieldInstanceRunTime", &Script_GetBattlefieldInstanceRunTime },
    { "GetBattlefieldEstimatedWaitTime", &Script_GetBattlefieldEstimatedWaitTime },
    { "GetBattlefieldTimeWaited", &Script_GetBattlefieldTimeWaited },
    { "CloseBattlefield", &Script_CloseBattlefield },
    { "RequestBattlefieldScoreData", &Script_RequestBattlefieldScoreData },
    { "GetNumBattlefieldScores", &Script_GetNumBattlefieldScores },
    { "GetBattlefieldScore", &Script_GetBattlefieldScore },
    { "GetBattlefieldWinner", &Script_GetBattlefieldWinner },
    { "SetBattlefieldScoreFaction", &Script_SetBattlefieldScoreFaction },
    { "LeaveBattlefield", &Script_LeaveBattlefield },
    { "GetNumBattlefieldStats", &Script_GetNumBattlefieldStats },
    { "GetBattlefieldStatInfo", &Script_GetBattlefieldStatInfo },
    { "GetBattlefieldStatData", &Script_GetBattlefieldStatData },
    { "RequestBattlefieldPositions", &Script_RequestBattlefieldPositions },
    { "GetNumBattlefieldPositions", &Script_GetNumBattlefieldPositions },
    { "GetBattlefieldPosition", &Script_GetBattlefieldPosition },
    { "GetNumBattlefieldFlagPositions", &Script_GetNumBattlefieldFlagPositions },
    { "GetBattlefieldFlagPosition", &Script_GetBattlefieldFlagPosition },
    { "GetNumBattlefieldVehicles", &Script_GetNumBattlefieldVehicles },
    { "GetBattlefieldVehicleInfo", &Script_GetBattlefieldVehicleInfo },
    { "CanJoinBattlefieldAsGroup", &Script_CanJoinBattlefieldAsGroup },
    { "GetBattlefieldMapIconScale", &Script_GetBattlefieldMapIconScale },
    { "GetBattlefieldTeamInfo", &Script_GetBattlefieldTeamInfo },
    { "GetBattlefieldArenaFaction", &Script_GetBattlefieldArenaFaction },
    { "SortBattlefieldScoreData", &Script_SortBattlefieldScoreData },
    { "HearthAndResurrectFromArea", &Script_HearthAndResurrectFromArea },
    { "CanHearthAndResurrectFromArea", &Script_CanHearthAndResurrectFromArea },
    { "GetNumBattlegroundTypes", &Script_GetNumBattlegroundTypes },
    { "GetBattlegroundInfo", &Script_GetBattlegroundInfo },
    { "RequestBattlegroundInstanceInfo", &Script_RequestBattlegroundInstanceInfo },
    { "GetNumArenaOpponents", &Script_GetNumArenaOpponents },
    { "BattlefieldMgrEntryInviteResponse", &Script_BattlefieldMgrEntryInviteResponse },
    { "BattlefieldMgrQueueRequest", &Script_BattlefieldMgrQueueRequest },
    { "BattlefieldMgrQueueInviteResponse", &Script_BattlefieldMgrQueueInviteResponse },
    { "BattlefieldMgrExitRequest", &Script_BattlefieldMgrExitRequest },
    { "GetWorldPVPQueueStatus", &Script_GetWorldPVPQueueStatus },
    { "GetHolidayBGHonorCurrencyBonuses", &Script_GetHolidayBGHonorCurrencyBonuses },
    { "GetRandomBGHonorCurrencyBonuses", &Script_GetRandomBGHonorCurrencyBonuses },
    { "SortBGList", &Script_SortBGList },
};

// VERIFY: 59f906df auto
// OFFSET: 0x0054A3E0
void BattlefieldInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
