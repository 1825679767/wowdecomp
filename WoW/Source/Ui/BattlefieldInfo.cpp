#include "BattlefieldInfo.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/BattlemasterListRec.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"

// STATIC: 0x00BEA4A8
WorldPvPQueueStatus CGBattlefieldInfo::m_worldPvPQueues[1];

// STATIC: 0x00BEA2C0
BattlefieldScoreSortCriteria CGBattlefieldInfo::m_scoreSortCriteria[17];

// STATIC: 0x00BEA720
TSGrowableArray<int32_t> CGBattlefieldInfo::m_battlegroundTypes;

// STATIC: 0x00BE9EF8
WOWGUID CGBattlefieldInfo::m_arenaOpponentPets[5][2];

// STATIC: 0x00BE9F48
WOWGUID CGBattlefieldInfo::m_arenaOpponents[5];

// STATIC: 0x00BEA580
uint32_t CGBattlefieldInfo::m_numScores;

// STATIC: 0x00BEA5AC
uint32_t CGBattlefieldInfo::m_numWorldStateStats;

// STATIC: 0x00BEA5B8
uint32_t CGBattlefieldInfo::m_numVehicles;

// STATIC: 0x00BEA578
uint32_t CGBattlefieldInfo::m_instanceStartTime;

// STATIC: 0x00BEA588
int32_t CGBattlefieldInfo::m_mapDone;

// STATIC: 0x00BEA58C
uint32_t CGBattlefieldInfo::m_winner;

// STATIC: 0x00BEA574
uint32_t CGBattlefieldInfo::m_instanceExpiration;

// STATIC: 0x00BEA0B0
char CGBattlefieldInfo::m_teamNames[2][96];

// STATIC: 0x00BEA594
uint32_t CGBattlefieldInfo::m_teamRatings[2];

// STATIC: 0x00BEA59C
uint32_t CGBattlefieldInfo::m_newTeamRatings[2];

// STATIC: 0x00BEA5A4
uint32_t CGBattlefieldInfo::m_avgMatchmakingRating[2];

// STATIC: 0x00ACD16C
int32_t CGBattlefieldInfo::m_activeIndex = -1;

// STATIC: 0x00BEA4B8
BattlefieldStatus CGBattlefieldInfo::m_status[2];

// STATIC: 0x00BEA564
uint32_t CGBattlefieldInfo::m_activeMapID;

// STATIC: 0x00BEA570
int32_t CGBattlefieldInfo::m_instanceType;

// STATIC: 0x00BEA538
int32_t CGBattlefieldInfo::m_selectedInstance;

// STATIC: 0x00BEA53C
int32_t CGBattlefieldInfo::m_bgTypeID;

// STATIC: 0x00BEA548
uint8_t CGBattlefieldInfo::m_hasHolidayWinToday;

// STATIC: 0x00BEA549
uint8_t CGBattlefieldInfo::m_hasRandomWinToday;

// STATIC: 0x00BEA54A
uint8_t CGBattlefieldInfo::m_arenaFaction;

// STATIC: 0x00BEA54C
int32_t CGBattlefieldInfo::m_holidayLossHonor;

// STATIC: 0x00BEA550
int32_t CGBattlefieldInfo::m_holidayWinHonor;

// STATIC: 0x00BEA554
int32_t CGBattlefieldInfo::m_holidayWinArena;

// STATIC: 0x00BEA558
int32_t CGBattlefieldInfo::m_randomLossHonor;

// STATIC: 0x00BEA55C
int32_t CGBattlefieldInfo::m_randomWinHonor;

// STATIC: 0x00BEA560
int32_t CGBattlefieldInfo::m_randomWinArena;

// STATIC: 0x00BEA5BC
TSGrowableArray<int32_t> CGBattlefieldInfo::m_instances;

// STATIC: 0x00ACD170
int32_t CGBattlefieldInfo::m_logFilterFaction = -1;

// STATIC: 0x00ACD174
int32_t CGBattlefieldInfo::m_allowJoin = 1;

// STATIC: 0x00BEA170
WOWGUID CGBattlefieldInfo::m_flagCarriers[2];

// STATIC: 0x00BEA368
BattlefieldScore* CGBattlefieldInfo::m_scores[80];

// STATIC: 0x00BEA540
uint32_t CGBattlefieldInfo::m_minLevel;

// STATIC: 0x00BEA544
uint32_t CGBattlefieldInfo::m_maxLevel;

// STATIC: 0x00BEA568
WOWGUID CGBattlefieldInfo::m_battlemaster;

// STATIC: 0x00BEA57C
uint32_t CGBattlefieldInfo::m_numScoreEntries;

// STATIC: 0x00BEA584
uint32_t CGBattlefieldInfo::m_nextLogUpdate;

// STATIC: 0x00BEA590
int32_t CGBattlefieldInfo::m_nameCachePending;

// STATIC: 0x00BEA5B0
uint32_t CGBattlefieldInfo::m_numPlayerPositions;

// STATIC: 0x00BEA5B4
uint32_t CGBattlefieldInfo::m_positionUpdateTime;

// STATIC: 0x00BEA710
C2Vector CGBattlefieldInfo::m_flagPositions[2];

// STATIC: 0x00BEA180
WOWGUID CGBattlefieldInfo::m_playerPositionGuids[40];

// STATIC: 0x00BEA348
int32_t CGBattlefieldInfo::m_worldStateStats[8];

// STATIC: 0x00BEA5D0
C2Vector CGBattlefieldInfo::m_playerPositions[40];

// VERIFY: 39950e3a auto
// OFFSET: 0x00549630
uint32_t GetArenaPetGUIDCount() {
    uint32_t count = 0;

    do {
        if (!CGBattlefieldInfo::m_arenaOpponents[count] && !CGBattlefieldInfo::m_arenaOpponentPets[count][0]) {
            break;
        }

        count++;
    } while (count < 5);

    return count;
}

// VERIFY: 4d274096 manual
// OFFSET: 0x00608180
WOWGUID CGBattlefieldInfo::GetArenaOpponent(uint32_t index) {
    if (index >= 5) {
        return WOWGUID{ 0 };
    }

    return CGBattlefieldInfo::m_arenaOpponents[index];
}

// VERIFY: 7a2896de manual
// OFFSET: 0x0060A2F0
WOWGUID CGBattlefieldInfo::GetArenaOpponentPet(uint32_t index) {
    if (index >= 5) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[index], TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CGUnitData* data = unit->m_unit;
        return data->m_unitFieldCharm ? data->m_unitFieldCharm : data->m_unitFieldSummon;
    }

    return CGBattlefieldInfo::m_arenaOpponentPets[index][0];
}

// VERIFY: 83965880 auto
// OFFSET: 0x0054CA30
static void ReleaseBattlefieldNameCacheRef(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!CGBattlefieldInfo::m_nameCachePending || !--CGBattlefieldInfo::m_nameCachePending) {
        BattlefieldNameCacheCallback(id, guid, param, success);
    }
}

// VERIFY: cf91ab36 manual
// OFFSET: 0x0054CA50
void CGBattlefieldInfo::EnterWorld() {
    CGBattlefieldInfo::m_selectedInstance = 0;
    CGBattlefieldInfo::m_bgTypeID = 0;
    CGBattlefieldInfo::m_minLevel = 0;
    CGBattlefieldInfo::m_maxLevel = 0;
    CGBattlefieldInfo::m_battlemaster = WOWGUID();
    CGBattlefieldInfo::m_nextLogUpdate = 0;
    CGBattlefieldInfo::m_mapDone = 0;
    CGBattlefieldInfo::m_winner = 0;
    CGBattlefieldInfo::m_logFilterFaction = -1;
    CGBattlefieldInfo::m_allowJoin = 1;

    if (CGBattlefieldInfo::m_nameCachePending) {
        for (uint32_t i = 0; i < CGBattlefieldInfo::m_numScoreEntries; i++) {
            g_nameDBCache.CancelCallback(CGBattlefieldInfo::m_scores[i]->m_guid.m_guid, &ReleaseBattlefieldNameCacheRef, nullptr);
        }
    }

    CGBattlefieldInfo::m_numScoreEntries = 0;
    CGBattlefieldInfo::m_flagPositions[0].x = 0.0f;
    CGBattlefieldInfo::m_numScores = 0;
    CGBattlefieldInfo::m_flagPositions[0].y = 0.0f;
    CGBattlefieldInfo::m_nameCachePending = 0;
    CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
    CGBattlefieldInfo::m_numWorldStateStats = 0;
    CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
    CGBattlefieldInfo::m_numPlayerPositions = 0;
    CGBattlefieldInfo::m_flagCarriers[0] = WOWGUID();
    CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
    CGBattlefieldInfo::m_positionUpdateTime = 0;

    for (uint32_t i = 0; i < 2; i++) {
        CGBattlefieldInfo::m_teamNames[i][0] = 0;
    }

    CGBattlefieldInfo::m_status[0].m_status = 0;
    CGBattlefieldInfo::m_status[1].m_status = 0;

    if (!CGGameUI::m_currentlyReloadingUI) {
        memset(CGBattlefieldInfo::m_arenaOpponentPets, 0, sizeof(CGBattlefieldInfo::m_arenaOpponentPets));

        for (uint32_t i = 0; i < 5; i++) {
            CGBattlefieldInfo::m_arenaOpponents[i] = WOWGUID();
        }

        for (int32_t i = 1; i <= 5; i++) {
            char unit[32];
            SStrPrintf(unit, sizeof(unit), "arena%d", i);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", unit, "cleared");
            SStrPrintf(unit, sizeof(unit), "arenapet%d", i);
            FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_OPPONENT_UPDATE, "%s%s", unit, "cleared");
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BATTLEFIELD_STATUS));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: b7727b77 manual
// OFFSET: 0x0054A450
int32_t CGBattlefieldInfo::GetPlayerPosition(uint32_t index, C3Vector* position) {
    if (index < CGBattlefieldInfo::m_numPlayerPositions) {
        float y = CGBattlefieldInfo::m_playerPositions[index].y;
        position->x = CGBattlefieldInfo::m_playerPositions[index].x;
        position->y = y;
        position->z = 0.0f;
        return 1;
    }

    return 0;
}

// VERIFY: ded4dbc4 auto
// OFFSET: 0x0054B3F0
int32_t Packet_MSG_BATTLEGROUND_PLAYER_POSITIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(CGBattlefieldInfo::m_numPlayerPositions);

    for (uint32_t i = 0; i < CGBattlefieldInfo::m_numPlayerPositions; i++) {
        msg->Get(CGBattlefieldInfo::m_playerPositionGuids[i].m_guid);
        msg->Get(CGBattlefieldInfo::m_playerPositions[i].x);
        msg->Get(CGBattlefieldInfo::m_playerPositions[i].y);
    }

    int32_t numFlags;
    msg->Get(reinterpret_cast<uint32_t&>(numFlags));

    if (numFlags > 0) {
        if (numFlags < 2) {
            CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
            CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
            CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
        }

        do {
            numFlags--;
            msg->Get(CGBattlefieldInfo::m_flagCarriers[numFlags].m_guid);
            msg->Get(CGBattlefieldInfo::m_flagPositions[numFlags].x);
            msg->Get(CGBattlefieldInfo::m_flagPositions[numFlags].y);
        } while (numFlags > 0);

        return 1;
    }

    CGBattlefieldInfo::m_flagPositions[0].x = 0.0f;
    CGBattlefieldInfo::m_flagPositions[0].y = 0.0f;
    CGBattlefieldInfo::m_flagCarriers[0] = WOWGUID();
    CGBattlefieldInfo::m_flagPositions[1].x = 0.0f;
    CGBattlefieldInfo::m_flagCarriers[1] = WOWGUID();
    CGBattlefieldInfo::m_flagPositions[1].y = 0.0f;
    return 1;
}

// VERIFY: 7ba135e2 manual
// OFFSET: 0x0054A650
int32_t QSortBattlefields(const void* a, const void* b) {
    int32_t idA = *static_cast<const int32_t*>(a);
    int32_t idB = *static_cast<const int32_t*>(b);

    if (idA == 32) {
        return -1;
    }

    if (idB == 32) {
        return 1;
    }

    BattlemasterListRec* recA = g_battlemasterListDB.GetRecord(idA);
    BattlemasterListRec* recB = g_battlemasterListDB.GetRecord(idB);

    if (GetWorldState(recA->m_holidayWorldState)) {
        return -1;
    }

    if (GetWorldState(recB->m_holidayWorldState)) {
        return 1;
    }

    if (recA->m_minLevel == recB->m_minLevel) {
        return idA - idB;
    }

    return recA->m_minLevel - recB->m_minLevel;
}

// VERIFY: a2090821 manual
// OFFSET: 0x0054E720
void CGBattlefieldInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_LIST, Packet_SMSG_BATTLEFIELD_LIST, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_STATUS, Packet_SMSG_BATTLEFIELD_STATUS, nullptr);
    ClientServices::SetMessageHandler(MSG_PVP_LOG_DATA, Packet_MSG_PVP_LOG_DATA, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_STATUS_QUEUED, Packet_SMSG_GROUP_JOINED_BATTLEGROUND, nullptr);
    ClientServices::SetMessageHandler(MSG_BATTLEGROUND_PLAYER_POSITIONS, Packet_MSG_BATTLEGROUND_PLAYER_POSITIONS, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEGROUND_PLAYER_JOINED, Packet_Group_3, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEGROUND_PLAYER_LEFT, Packet_Group_3, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_OPPONENT_UPDATE, Packet_SMSG_ARENA_OPPONENT_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_PORT_DENIED, Packet_SMSG_BATTLEFIELD_PORT_DENIED, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, Packet_SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_ENTERING, Packet_SMSG_BATTLEFIELD_MGR_ENTERED, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, Packet_SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_EJECT_PENDING, Packet_SMSG_BATTLEFIELD_MGR_EJECT_PENDING, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_EJECTED, Packet_SMSG_BATTLEFIELD_MGR_EJECTED, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, Packet_SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, nullptr);
    ClientServices::SetMessageHandler(SMSG_BATTLEFIELD_MGR_STATE_CHANGE, Packet_SMSG_BATTLEFIELD_MGR_STATE_CHANGE, nullptr);

    CGBattlefieldInfo::m_activeIndex = -1;
    CGBattlefieldInfo::m_activeMapID = 0;
    CGBattlefieldInfo::m_battlemaster.m_guid = 0;
    CGBattlefieldInfo::m_instanceExpiration = 0;
    CGBattlefieldInfo::m_instanceStartTime = 0;
    CGBattlefieldInfo::m_nameCachePending = 0;
    CGBattlefieldInfo::m_numScoreEntries = 0;
    CGBattlefieldInfo::m_numScores = 0;
    CGBattlefieldInfo::m_logFilterFaction = -1;
    CGBattlefieldInfo::m_numVehicles = 0;
    CGBattlefieldInfo::m_allowJoin = 1;

    for (uint32_t i = 0; i < 2; i++) {
        CGBattlefieldInfo::m_status[i].m_status = 0;
        CGBattlefieldInfo::m_status[i].m_queueData = 0;
        CGBattlefieldInfo::m_status[i].m_bgTypeID = 0;
        CGBattlefieldInfo::m_status[i].m_mapID = 0;
    }

    for (uint32_t i = 0; i < 1; i++) {
        CGBattlefieldInfo::m_worldPvPQueues[i].m_status = 0;
        CGBattlefieldInfo::m_worldPvPQueues[i].m_areaID = 0;
        CGBattlefieldInfo::m_worldPvPQueues[i].m_queueID = 0;
    }

    for (uint32_t i = 0; i < 80; i++) {
        CGBattlefieldInfo::m_scores[i] = static_cast<BattlefieldScore*>(SMemAlloc(sizeof(BattlefieldScore), __FILE__, __LINE__, 0));
    }

    for (uint32_t i = 0; i < 17; i++) {
        CGBattlefieldInfo::m_scoreSortCriteria[i].m_column = i;
        CGBattlefieldInfo::m_scoreSortCriteria[i].m_descending = 0;
    }

    for (int32_t i = 0; i < g_battlemasterListDB.m_numRecords; i++) {
        BattlemasterListRec* rec = i < 0 || i >= g_battlemasterListDB.m_numRecords ? nullptr : &g_battlemasterListDB.m_records[i];

        if (rec && rec->m_instanceType == 3) {
            CGBattlefieldInfo::m_battlegroundTypes.New(rec->m_id);
        }
    }

    qsort(CGBattlefieldInfo::m_battlegroundTypes.m_data, CGBattlefieldInfo::m_battlegroundTypes.Count(), sizeof(int32_t), QSortBattlefields);
    memset(CGBattlefieldInfo::m_arenaOpponentPets, 0, sizeof(CGBattlefieldInfo::m_arenaOpponentPets));
}

// VERIFY: 0c16867d manual
// OFFSET: 0x0054CF60
void CGBattlefieldInfo::RequestPlayerPositions() {
    if (CGBattlefieldInfo::m_activeIndex < 0 || !CGBattlefieldInfo::m_status[CGBattlefieldInfo::m_activeIndex].m_bgTypeID) {
        CGBattlefieldInfo::m_numPlayerPositions = 0;
        return;
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (CGBattlefieldInfo::m_positionUpdateTime && static_cast<int32_t>(now - CGBattlefieldInfo::m_positionUpdateTime) < 0) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_BATTLEGROUND_PLAYER_POSITIONS));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGBattlefieldInfo::m_positionUpdateTime = now + 5000;
}
