#include "QuestLog.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <ctime>
#include <cstdlib>
#include <malloc.h>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/QuestInfoRec.h"
#include "WoW/Source/DB/Rec/QuestSortRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

// STATIC: 0x00C23618
int32_t CGQuestLog::m_sortTypes[25];

// STATIC: 0x00C23AD4
uint32_t CGQuestLog::m_numSortTypes;

// STATIC: 0x00C23AEC
int32_t CGQuestLog::m_questResetTime;

// STATIC: 0x00C23AF0
uint8_t CGQuestLog::m_trackedQuestsLoaded;

// STATIC: 0x00C23AD0
int32_t CGQuestLog::m_numQuest;

// STATIC: 0x00C23AF4
static uint32_t s_questCallbackCount;

// STATIC: 0x00AD1018
float CGQuestLog::m_questIconOverlapPushDist = 18.0f;

// STATIC: 0x00C23AD8
int32_t CGQuestLog::m_selectedQuest;

// STATIC: 0x00AD1014
float CGQuestLog::m_questIconOverlapDist = 12.0f;

// STATIC: 0x00A1C4A0
static const int32_t s_questGreenRange[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

// STATIC: 0x00C23680
QuestWatch CGQuestLog::m_questWatches[25];

// STATIC: 0x00C237B0
QuestLogEntry CGQuestLog::m_quests[50];

// STATIC: 0x00C23AE4
uint32_t CGQuestLog::m_numShownQuests;

// STATIC: 0x00C23ADC
int32_t CGQuestLog::m_abandonQuest;

// STATIC: 0x00C23AE0
uint32_t CGQuestLog::m_numQuestWatches;

// STATIC: 0x00C23AE8
int32_t CGQuestLog::m_timeOffset;

// STATIC: 0x00C23AF8
int32_t CGQuestLog::m_nextQuestUpdate;

// STATIC: 0x00C23AFC
static CVar* s_cvPOIShiftComplete;

// STATIC: 0x00C23B00
QuestPOIInfo CGQuestLog::m_questPOIs[25];

// VERIFY: 45bd143e manual
// OFFSET: 0x005E6FA0
void CGQuestLog::EnterWorld() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_TIME));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_questCallbackCount = 0;
    CGQuestLog::Update(1);
}

// VERIFY: ff43712e manual
// OFFSET: 0x005E2860
int32_t CGQuestLog::GetQuestGreenRange() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(player->m_unit->m_unitFieldLevel) / 5;

    if (index >= 20) {
        index = 19;
    }

    return s_questGreenRange[index];
}

// VERIFY: 673a158b manual
// OFFSET: 0x005DEC40
int32_t CGQuestLog::GetQuestAt(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    return CGQuestLog::m_quests[index].m_questId;
}

// VERIFY: 5dca5cb5 manual
// OFFSET: 0x005E71A0
void CGQuestLog::InitializeGame() {
    CGQuestLog::m_selectedQuest = 0;
    CGQuestLog::m_abandonQuest = 0;
    CGQuestLog::m_numQuest = 0;
    CGQuestLog::m_numShownQuests = 0;
    memset(CGQuestLog::m_quests, 0, sizeof(CGQuestLog::m_quests));
    CGQuestLog::m_numQuestWatches = 0;
    memset(CGQuestLog::m_questWatches, 0, sizeof(CGQuestLog::m_questWatches));
    CGQuestLog::m_timeOffset = 0;
    CGQuestLog::m_nextQuestUpdate = 0;

    for (uint32_t i = 0; i < 25; i++) {
        QuestPOIInfo* poi = &CGQuestLog::m_questPOIs[i];
        poi->m_questId = 0;
        poi->unk_0014 = 0;
        poi->unk_0018 = 0;
        poi->unk_001C = 0;
        poi->unk_0020 = 0;
        poi->unk_0024 = 0;
        poi->unk_0029 = 0;
        poi->unk_0028 = 0;
        poi->unk_002C = 0;
    }

    CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    flags.m_forceMipTracking = 1;
    flags.m_renderTarget = 1;
    ClientServices::SetMessageHandler(SMSG_QUERY_TIME_RESPONSE, Packet_SMSG_QUERY_TIME_RESPONSE, nullptr);
    s_cvPOIShiftComplete = CVar::Register("POIShiftComplete", nullptr, 0, "0.6", nullptr, 0, false, nullptr, false);
    ConsoleCommandRegister("UpdatePOI", UpdateQuestPOIHandler, CATEGORY_GAME, "Updates Quest POI from User Server");
}

// VERIFY: 988b76a4 auto
// OFFSET: 0x005DEBD0
int32_t CGQuestLog::GetSelectedLogEntry() {
    uint32_t index = 0;

    for (; index < static_cast<uint32_t>(CGQuestLog::m_numQuest); index++) {
        if (!CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_questId == CGQuestLog::m_selectedQuest) {
            break;
        }
    }

    if (index == static_cast<uint32_t>(CGQuestLog::m_numQuest)) {
        return -1;
    }

    return index;
}

// VERIFY: 98372535 manual
// OFFSET: 0x005DFFA0
void CGQuestLog::SetSelectedQuest(int32_t index) {
    if (CGQuestLog::m_selectedQuest) {
        DisplayQuestPoint(0.0f, 0.0f, 0, "");
    }

    if (index < 0 || index >= CGQuestLog::m_numQuest) {
        CGQuestLog::m_selectedQuest = 0;
    } else if (!CGQuestLog::m_quests[index].m_isHeader) {
        CGQuestLog::m_selectedQuest = CGQuestLog::m_quests[index].m_questId;
        AddQuestPointDisplay();
    }
}

// VERIFY: 4aaec04c manual
// OFFSET: 0x005E57E0
int32_t CGQuestLog::SetQuestExpired(uint32_t index) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest) || CGQuestLog::IsQuestCompleteEx(index, 1)) {
        return 0;
    }

    CGQuestLog::m_quests[index].m_expired = 1;

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::QueryQuestGiverStatusMultiple();
    }

    return 1;
}

// VERIFY: 5dcc8e24 auto
// OFFSET: 0x005DE980
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    QuestCache* quest = g_questDBCache.GetRecord(id, WOWGUID(), nullptr, nullptr, false);

    if (quest) {
        CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
    }
}

// VERIFY: 6e014a22 auto
// OFFSET: 0x005DE930
void QuestQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    } else {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
    }
}

// VERIFY: 88d84110 auto
// OFFSET: 0x005DEE30
int32_t CGQuestLog::IsQuestFailed(uint32_t index) {
    return index < static_cast<uint32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired;
}

// VERIFY: 1f30d3fb manual
// OFFSET: 0x005DEEB0
int32_t CGQuestLog::GetQuestIndex(int32_t questId) {
    for (int32_t i = 0; i < 50; i++) {
        if (!CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId == questId) {
            return i + 1;
        }
    }

    return 0;
}

// VERIFY: 728ea3e9 manual
// OFFSET: 0x005E0EA0
int32_t CGQuestLog::IsQuestCompleteEx(uint32_t index, int32_t a2) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t slot = CGQuestLog::m_quests[index].m_logSlot;
    PlayerQuestLogEntry* entry = slot >= 0 && slot <= 25 ? &player->m_player->m_playerQuestLog[slot] : nullptr;
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);

    if (!quest || !entry || (entry->m_state & 2) || CGQuestLog::IsQuestFailed(index)) {
        return 0;
    }

    uint32_t special = quest->m_flags & 6;

    if (!special && !quest->m_friendlyFactionId && !quest->m_hostileFactionId && !quest->m_rewardMoney) {
        int32_t hasObjectives = 0;

        for (uint32_t i = 0; i < 4; i++) {
            if (quest->m_objectiveId[i]) {
                hasObjectives = 1;
                break;
            }
        }

        for (uint32_t i = 0; i < 6; i++) {
            if (quest->m_collectItemId[i]) {
                hasObjectives = 1;
                break;
            }
        }

        if (a2 && !hasObjectives && !quest->m_requiredPlayersKilled) {
            return 0;
        }
    }

    if (special && !(entry->m_state & 1)) {
        return 0;
    }

    if (quest->m_friendlyFactionId && GetRepListRepValue(quest->m_friendlyFactionId) < static_cast<int32_t>(quest->m_friendlyFactionAmount)) {
        return 0;
    }

    if (quest->m_hostileFactionId && GetRepListRepValue(quest->m_hostileFactionId) > static_cast<int32_t>(quest->m_hostileFactionAmount)) {
        return 0;
    }

    if (quest->m_rewardMoney < 0 && player->GetCoinage() < static_cast<uint32_t>(-quest->m_rewardMoney)) {
        return 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (quest->m_objectiveId[i] && entry->m_counts[i] < quest->m_objectiveRequiredCount[i]) {
            return 0;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (quest->m_collectItemId[i] && player->m_bag.GetItemTypeCount(quest->m_collectItemId[i], 8) < static_cast<int32_t>(quest->m_collectItemCount[i])) {
            return 0;
        }
    }

    return static_cast<int32_t>(entry->m_counts[0]) >= static_cast<int32_t>(quest->m_requiredPlayersKilled);
}

// VERIFY: 7a39ad3c manual
// OFFSET: 0x005E10B0
bool CGQuestLog::HasNoObjectives(uint32_t index) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return false;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    int32_t slot = CGQuestLog::m_quests[index].m_logSlot;
    PlayerQuestLogEntry* entry = static_cast<uint32_t>(slot) <= 25 ? &player->m_player->m_playerQuestLog[slot] : nullptr;
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);

    if (!quest || !entry || (entry->m_state & 2) || CGQuestLog::IsQuestFailed(index)) {
        return false;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (quest->m_objectiveId[i]) {
            return false;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (quest->m_collectItemId[i] && (quest->m_collectItemId[i] != quest->m_startingItemId || static_cast<int32_t>(quest->m_collectItemCount[i]) > 1)) {
            return false;
        }
    }

    return !quest->m_requiredPlayersKilled && !(quest->m_flags & 6) && !quest->m_friendlyFactionId && !quest->m_hostileFactionId && quest->m_rewardMoney >= 0;
}

// VERIFY: e4a82c9a manual
// OFFSET: 0x005DED30
int32_t GetQuestSuggestedPlayers(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);

    if (!quest) {
        return 0;
    }

    return quest->m_suggestedPlayers;
}

// VERIFY: 00b5ee18 manual
// OFFSET: 0x005E0CC0
void CGQuestLog::AbandonSelectedQuest() {
    if (!CGQuestLog::m_abandonQuest || !ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    for (int32_t i = 0; i < 50; i++) {
        if (!CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId == CGQuestLog::m_abandonQuest) {
            CGPlayer_C::QuestLogRemoveQuest(static_cast<uint8_t>(CGQuestLog::m_quests[i].m_logSlot));
            return;
        }
    }
}

// VERIFY: 45625162 manual
// OFFSET: 0x005E1300
int32_t GetNumQuestLeaderBoards(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    int32_t count = 0;
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, callback, param, false);

    if (quest) {
        if (quest->m_endText[0]) {
            count = 1;
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (quest->m_objectiveId[i]) {
                count++;
            }
        }

        for (uint32_t i = 0; i < 6; i++) {
            if (static_cast<int32_t>(quest->m_collectItemId[i]) > 0 && (quest->m_collectItemId[i] != quest->m_startingItemId || static_cast<int32_t>(quest->m_collectItemCount[i]) > 1)) {
                count++;
            }
        }

        if (quest->m_friendlyFactionId) {
            count++;
        }

        if (quest->m_requiredPlayersKilled) {
            count++;
        }

        if (count) {
            return count;
        }
    }

    return CGQuestLog::IsQuestCompleteEx(CGQuestLog::GetQuestIndex(questId) - 1, 1) ? 1 : count;
}

// VERIFY: 390e816f manual
// OFFSET: 0x005E1FF0
int32_t CountQuestItemDropsNeeded(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t count = 0;
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, callback, param, false);

    if (quest) {
        for (uint32_t i = 0; i < 4; i++) {
            int32_t itemId = quest->m_intermediateItemId[i];

            if (itemId > 0 && (static_cast<uint32_t>(itemId) != quest->m_startingItemId || static_cast<int32_t>(quest->m_intermediateItemCount[i]) > 1) && !player->m_bag.GetItemTypeCount(itemId, 8)) {
                count++;
            }
        }
    }

    return count;
}

// VERIFY: 89b5e6a8 manual
// OFFSET: 0x005E64D0
int32_t CGQuestLog::AddQuestWatch(int32_t questId, int32_t duration) {
    int32_t now = static_cast<int32_t>(time(nullptr));
    int32_t i;

    for (i = 0; i < 50; i++) {
        if (!CGQuestLog::m_quests[i].m_isHeader && CGQuestLog::m_quests[i].m_questId == questId) {
            break;
        }
    }

    if (i >= 50) {
        return now;
    }

    for (uint32_t j = 0; j < CGQuestLog::m_numQuestWatches; j++) {
        if (CGQuestLog::m_questWatches[j].m_questId == questId) {
            if (CGQuestLog::m_questWatches[j].m_expireTime > 0) {
                CGQuestLog::m_questWatches[j].m_expireTime = duration ? now + duration : 0;
            }

            return now;
        }
    }

    if (CGQuestLog::m_numQuestWatches < 25) {
        uint32_t index = CGQuestLog::m_numQuestWatches;
        CGQuestLog::m_questWatches[index].m_questId = questId;
        CGQuestLog::m_questWatches[index].m_expireTime = duration ? duration + now : 0;
        CGQuestLog::m_numQuestWatches = index + 1;
        return CGQuestLog::SortQuestWatches();
    }

    return now;
}

// VERIFY: b5dd4e6f auto
// OFFSET: 0x005DE8F0
static void QuestQueryCounterCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Invalid quest log entry", DEFAULT_COLOR);
        return;
    }

    if (s_questCallbackCount == 0 || --s_questCallbackCount == 0) {
        CGQuestLog::Update(1);
    }
}

// VERIFY: 51715ead manual
// OFFSET: 0x005DEA60
void CGQuestLog::RemoveQuestWatch(int32_t questId) {
    uint32_t count = CGQuestLog::m_numQuestWatches;
    uint32_t i;

    if (!count) {
        return;
    }

    for (i = 0; CGQuestLog::m_questWatches[i].m_questId != questId; i++) {
        if (i + 1 >= CGQuestLog::m_numQuestWatches) {
            return;
        }
    }

    for (; i < CGQuestLog::m_numQuestWatches - 1; i++) {
        CGQuestLog::m_questWatches[i].m_questId = CGQuestLog::m_questWatches[i + 1].m_questId;
        CGQuestLog::m_questWatches[i].m_expireTime = CGQuestLog::m_questWatches[i + 1].m_expireTime;
    }

    CGQuestLog::m_questWatches[count - 1].m_questId = 0;
    CGQuestLog::m_numQuestWatches = count - 1;
    CGQuestLog::m_questWatches[count - 1].m_expireTime = 0;
}

// VERIFY: cedecb8a auto
// OFFSET: 0x005DEAE0
int32_t GetQuestSortId(uint32_t index) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest)) {
        return -1;
    }

    int32_t sortType = CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::m_quests[index].m_isHeader) {
        WOWGUID none = { 0 };
        QuestCache* quest = g_questDBCache.GetRecord(sortType, none, nullptr, nullptr, false);

        if (!quest) {
            return -1;
        }

        sortType = quest->m_areaIdOrSortId;
    }

    for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
        if (CGQuestLog::m_sortTypes[i] == sortType) {
            return i;
        }
    }

    return -1;
}

// VERIFY: d0925147 manual
// OFFSET: 0x005DEC70
const char* GetQuestTitleText(int32_t questId) {
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCallback, nullptr, false);

    if (!quest) {
        return nullptr;
    }

    return quest->m_name;
}

// VERIFY: f8dc9db0 manual
// OFFSET: 0x005DECC0
uint32_t IsDailyQuest(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);

    if (!quest) {
        return 0;
    }

    return quest->m_flags & 0x1000;
}

// VERIFY: e94a9f5a manual
// OFFSET: 0x005DEF60
void CGQuestLog::ShiftQuestWatches(uint32_t from, uint32_t to) {
    int32_t step;

    if (to > from && to < CGQuestLog::m_numQuestWatches) {
        step = 1;
    } else if (to < from && from < CGQuestLog::m_numQuestWatches) {
        step = -1;
    } else {
        return;
    }

    QuestWatch moved = CGQuestLog::m_questWatches[from];

    for (uint32_t i = from; i != to; i += step) {
        CGQuestLog::m_questWatches[i] = CGQuestLog::m_questWatches[i + step];
    }

    CGQuestLog::m_questWatches[to] = moved;
}

// VERIFY: db83baeb manual
// OFFSET: 0x005DFDC0
static int32_t QSortQuestSortTypes(const void* a, const void* b) {
    int32_t sortA = *static_cast<const int32_t*>(a);
    int32_t sortB = *static_cast<const int32_t*>(b);

    if (sortA == sortB) {
        return 0;
    }

    if (!sortA) {
        return -1;
    }

    if (!sortB) {
        return 1;
    }

    char nameA[256] = { 0 };
    char nameB[256] = { 0 };

    if (sortA >= 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(sortA);

        if (area) {
            SStrCopy(nameA, area->m_areaName, sizeof(nameA));
        }
    } else {
        QuestSortRec* sort = g_questSortDB.GetRecord(-sortA);

        if (sort) {
            SStrCopy(nameA, sort->m_sortName, sizeof(nameA));
        }
    }

    if (sortB >= 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(sortB);

        if (area) {
            SStrCopy(nameB, area->m_areaName, sizeof(nameB));
        }
    } else {
        QuestSortRec* sort = g_questSortDB.GetRecord(-sortB);

        if (sort) {
            SStrCopy(nameB, sort->m_sortName, sizeof(nameB));
        }
    }

    return SStrSortUTF8I(nameA, nameB, STORM_MAX_STR);
}

// VERIFY: 651d4dbc manual
// OFFSET: 0x005E0000
const char* GetQuestTitle(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest) {
        return nullptr;
    }

    int32_t id = CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::m_quests[index].m_isHeader) {
        return GetQuestTitleText(id);
    }

    if (id > 0) {
        AreaTableRec* area = g_areaTableDB.GetRecord(id);

        if (area) {
            return area->m_areaName;
        }
    } else if (id < 0) {
        QuestSortRec* sort = g_questSortDB.GetRecord(-id);

        if (sort) {
            return sort->m_sortName;
        }
    } else {
        return "Missing header! (quest designers)";
    }

    return nullptr;
}

// VERIFY: 12e6b2ec manual
// OFFSET: 0x005E0070
const char* GetQuestTag(int32_t index) {
    if (index < 0 || index >= CGQuestLog::m_numQuest || CGQuestLog::m_quests[index].m_isHeader) {
        return nullptr;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);
    QuestInfoRec* info;

    if (!quest || !(info = g_questInfoDB.GetRecord(quest->m_infoId))) {
        return nullptr;
    }

    return info->m_infoName;
}

// VERIFY: 074af98c auto
// OFFSET: 0x005E00E0
int32_t CGQuestLog::IsSelectedQuestExpired() {
    uint32_t index = CGQuestLog::GetSelectedLogEntry();
    return index < static_cast<uint32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired;
}

// VERIFY: f903a683 manual
// OFFSET: 0x005E08F0
static int32_t QSortQuestLogEntries(const void* a, const void* b) {
    QuestLogEntry entryA = *static_cast<const QuestLogEntry*>(a);
    QuestLogEntry entryB = *static_cast<const QuestLogEntry*>(b);
    int32_t idA = entryA.m_questId;
    int32_t idB = entryB.m_questId;
    int32_t headerA = entryA.m_isHeader;
    int32_t headerB = entryB.m_isHeader;
    WOWGUID none = { 0 };
    QuestCache* questA = headerA ? nullptr : g_questDBCache.GetRecord(idA, none, nullptr, nullptr, false);
    QuestCache* questB = headerB ? nullptr : g_questDBCache.GetRecord(idB, none, nullptr, nullptr, false);
    uint32_t sortA = 0;
    uint32_t sortB;
    uint32_t i;

    if (headerA) {
        for (i = 0; i < 25 && CGQuestLog::m_sortTypes[i] != idA; i++) {
        }
    } else {
        if (!questA) {
            return 0;
        }

        for (i = 0; i < 25 && CGQuestLog::m_sortTypes[i] != static_cast<int32_t>(questA->m_areaIdOrSortId); i++) {
        }
    }

    if (i < 25) {
        sortA = i;
    }

    if (headerB) {
        for (i = 0; i < 25 && CGQuestLog::m_sortTypes[i] != idB; i++) {
        }
    } else {
        if (!questB) {
            return 0;
        }

        for (i = 0; i < 25 && CGQuestLog::m_sortTypes[i] != static_cast<int32_t>(questB->m_areaIdOrSortId); i++) {
        }
    }

    sortB = i < 25 ? i : 0;
    int32_t hiddenA = !headerA && sortA < 25 && !((1 << sortA) & s_cvQuestLogCollapseFilter->m_intValue);
    int32_t hiddenB = !headerB && sortB < 25 && !((1 << sortB) & s_cvQuestLogCollapseFilter->m_intValue);

    if (hiddenA) {
        if (hiddenB) {
            return 0;
        }

        return 2 * hiddenA - 1;
    }

    if (hiddenB) {
        return 2 * hiddenA - 1;
    }

    if (sortA != sortB) {
        return sortA < sortB ? -1 : 1;
    }

    if (headerA) {
        return -1;
    }

    if (headerB) {
        return 1;
    }

    int32_t levelA = questA->m_level;
    int32_t levelB = questB->m_level;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        if (levelA == -1) {
            levelA = player->m_unit->m_unitFieldLevel;
        }

        if (levelB == -1) {
            levelB = player->m_unit->m_unitFieldLevel;
        }
    }

    if (levelA == levelB) {
        return SStrSortUTF8I(questA->m_name, questB->m_name, STORM_MAX_STR);
    }

    return 2 * (levelA >= levelB) - 1;
}

// VERIFY: 01ac7701 auto
// OFFSET: 0x005E0B80
void CGQuestLog::FilterAndSortQuests() {
    qsort(CGQuestLog::m_quests, CGQuestLog::m_numQuest, sizeof(QuestLogEntry), QSortQuestLogEntries);
    CGQuestLog::m_numShownQuests = CGQuestLog::m_numQuest;

    for (uint32_t i = 0; i < static_cast<uint32_t>(CGQuestLog::m_numQuest); i++) {
        if (CGQuestLog::m_quests[i].m_isHeader) {
            continue;
        }

        uint32_t sortId = GetQuestSortId(i);

        if (sortId < 25 && !((1 << sortId) & s_cvQuestLogCollapseFilter->m_intValue)) {
            CGQuestLog::m_numShownQuests--;
        }
    }
}

// VERIFY: 8af272a7 auto
// OFFSET: 0x005E0C00
void CGQuestLog::CollapseHeader(uint32_t index, int32_t collapse) {
    if (index < static_cast<uint32_t>(CGQuestLog::m_numQuest) && CGQuestLog::m_quests[index].m_isHeader) {
        uint32_t i;

        for (i = 0; i < CGQuestLog::m_numSortTypes; i++) {
            if (CGQuestLog::m_sortTypes[i] == CGQuestLog::m_quests[index].m_questId) {
                break;
            }
        }

        if (i == CGQuestLog::m_numSortTypes) {
            return;
        }

        int32_t filter = s_cvQuestLogCollapseFilter->m_intValue;
        int32_t bit = 1 << i;
        s_cvQuestLogCollapseFilter->Set(collapse ? filter & ~bit : filter | bit, true, false, false, true);
    } else {
        s_cvQuestLogCollapseFilter->Set((collapse != 0) - 1, true, false, false, true);
    }

    CGQuestLog::FilterAndSortQuests();
    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
}

// VERIFY: 89bc5f8b manual
// OFFSET: 0x005E0D20
int32_t GetQuestLevel(int32_t questId) {
    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCallback, nullptr, false);

    if (!quest) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && quest->m_level == 0xFFFFFFFF) {
        return player->m_unit->m_unitFieldLevel;
    }

    return quest->m_level;
}

// VERIFY: 92305a9d auto
// OFFSET: 0x005E0DA0
CGItem_C* CGQuestLog::GetQuestSpecialItem(uint32_t index) {
    if (index >= static_cast<uint32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return nullptr;
    }

    WOWGUID none = { 0 };
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_quests[index].m_questId, none, QuestQueryCallback, nullptr, false);

    if (!quest || (!quest->m_startingItemId && !(quest->m_flags & 0x20000))) {
        return nullptr;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return nullptr;
    }

    CGBag_C* bag = &player->m_bag;
    CGItem_C* item = bag->FindItemOfType(quest->m_startingItemId, 0);

    if (item && item->CanBeUsed()) {
        return item;
    }

    if (!(quest->m_flags & 0x20000)) {
        return nullptr;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (!quest->m_intermediateItemId[i]) {
            continue;
        }

        item = bag->FindItemOfType(quest->m_intermediateItemId[i], 0);

        if (item && item->CanBeUsed()) {
            return item;
        }
    }

    return nullptr;
}

// VERIFY: 716d0e51 manual
// OFFSET: 0x005E28B0
bool CGQuestLog::HasQuestPOI(int32_t questId) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || questId <= 0) {
        return false;
    }

    for (uint32_t i = 0; i < 25; i++) {
        if (CGQuestLog::m_questPOIs[i].m_questId == questId) {
            return true;
        }
    }

    return false;
}

// VERIFY: 86468e46 manual
// OFFSET: 0x005E6940
void CGQuestLog::Update(int32_t a1) {
    if (!a1) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
        return;
    }

    if (s_questCallbackCount) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t filter = s_cvQuestLogCollapseFilter->m_intValue;
    uint32_t numOldSortTypes = CGQuestLog::m_numSortTypes;
    int32_t* oldSortTypes = static_cast<int32_t*>(alloca(numOldSortTypes * sizeof(int32_t)));

    for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
        oldSortTypes[i] = CGQuestLog::m_sortTypes[i];

        if ((1 << i) & filter) {
            oldSortTypes[i] = -oldSortTypes[i];
        }
    }

    memset(CGQuestLog::m_quests, 0, sizeof(CGQuestLog::m_quests));
    memset(CGQuestLog::m_sortTypes, 0, sizeof(CGQuestLog::m_sortTypes));
    CGQuestLog::m_numQuest = 0;
    CGQuestLog::m_numSortTypes = 0;
    s_questCallbackCount = 0;
    int32_t numUnknown = 0;
    int32_t unknown[25] = { 0 };
    int32_t timeOffset = CGQuestLog::m_timeOffset;

    for (uint32_t slot = 0; slot < 25; slot++) {
        PlayerQuestLogEntry* entry = slot > 25 ? nullptr : &player->m_player->m_playerQuestLog[slot];
        int32_t questId = entry->m_questId;

        if (questId <= 0) {
            continue;
        }

        WOWGUID none = { 0 };
        QuestCache* quest = g_questDBCache.GetRecord(questId, none, QuestQueryCounterCallback, nullptr, false);

        if (!quest) {
            s_questCallbackCount++;
            continue;
        }

        if (!CGQuestLog::HasQuestPOI(questId)) {
            numUnknown++;
            unknown[slot] = questId;
        }

        CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_questId = questId;
        CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_logSlot = slot;
        CGQuestLog::m_numQuest++;

        if (entry->m_time && !(entry->m_state & 2) && timeOffset + static_cast<int32_t>(entry->m_time) - static_cast<int32_t>(time(nullptr)) - 1 < 0) {
            CGQuestLog::SetQuestExpired(CGQuestLog::m_numQuest - 1);
        }

        uint32_t i;

        for (i = 0; i < CGQuestLog::m_numSortTypes; i++) {
            if (CGQuestLog::m_sortTypes[i] == static_cast<int32_t>(quest->m_areaIdOrSortId)) {
                break;
            }
        }

        if (i == CGQuestLog::m_numSortTypes) {
            CGQuestLog::m_sortTypes[i] = quest->m_areaIdOrSortId;
            CGQuestLog::m_numSortTypes++;
            CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_questId = quest->m_areaIdOrSortId;
            CGQuestLog::m_quests[CGQuestLog::m_numQuest].m_isHeader = 1;
            CGQuestLog::m_numQuest++;
        }
    }

    qsort(CGQuestLog::m_sortTypes, CGQuestLog::m_numSortTypes, sizeof(int32_t), QSortQuestSortTypes);

    for (uint32_t i = 0; i < CGQuestLog::m_numSortTypes; i++) {
        uint32_t j;

        for (j = 0; j < numOldSortTypes; j++) {
            if (oldSortTypes[j] == CGQuestLog::m_sortTypes[i]) {
                filter &= ~(1 << i);
                break;
            }

            if (oldSortTypes[j] == -CGQuestLog::m_sortTypes[i]) {
                filter |= 1 << i;
                break;
            }
        }

        if (j >= numOldSortTypes) {
            filter |= 1 << i;
        }
    }

    s_cvQuestLogCollapseFilter->Set(filter, true, false, false, true);
    CGQuestLog::FilterAndSortQuests();

    if (!CGQuestLog::m_trackedQuestsLoaded && CGQuestLog::m_numQuest) {
        CGQuestLog::LoadTrackedQuests();
    }

    bool found = true;

    for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
        if (!found) {
            i--;
        }

        found = false;

        for (int32_t j = 0; j < CGQuestLog::m_numQuest; j++) {
            if (!CGQuestLog::m_quests[j].m_isHeader && CGQuestLog::m_quests[j].m_questId == CGQuestLog::m_questWatches[i].m_questId) {
                found = true;
                break;
            }
        }

        if (!found) {
            CGQuestLog::RemoveQuestWatch(CGQuestLog::m_questWatches[i].m_questId);
        }
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);

    if (numUnknown) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_QUEST_POI_QUERY));
        msg.Put(static_cast<uint32_t>(numUnknown));

        for (uint32_t i = 0; i < 25; i++) {
            if (unknown[i]) {
                msg.Put(static_cast<uint32_t>(unknown[i]));
            }
        }

        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (CGQuestLog::m_nextQuestUpdate && static_cast<int32_t>(time(nullptr)) >= CGQuestLog::m_nextQuestUpdate) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_QUERY_TIME));
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGQuestLog::m_nextQuestUpdate = 0;
    }
}
