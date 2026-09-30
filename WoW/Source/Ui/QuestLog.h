#ifndef WOW_SOURCE_UI_QUESTLOG_H
#define WOW_SOURCE_UI_QUESTLOG_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CGItem_C;
class CDataStore;

struct lua_State;

class QuestLogLeaderBoardParams;

struct QuestLogEntry {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ int32_t m_logSlot;
    /* 0008 */ int32_t m_isHeader;
    /* 000C */ int32_t m_expired;
};

struct QuestPOIPoint {
    /* 0000 */ int32_t m_x;
    /* 0004 */ int32_t m_y;
};

struct QuestPOIBlob {
    /* 0000 */ TSGrowableArray<QuestPOIPoint> m_points;
    /* 0010 */ int32_t m_centerX;
    /* 0014 */ int32_t m_centerY;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
};

struct QuestPOIInfo {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ TSGrowableArray<QuestPOIBlob> m_blobs;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint8_t unk_0028;
    /* 0029 */ uint8_t unk_0029;
    /* 002C */ uint32_t unk_002C;
};

struct QuestWatch {
    /* 0000 */ int32_t m_questId;
    /* 0004 */ int32_t m_expireTime;
    /* 0008 */ uint32_t unk_0008;
};

void QuestQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void ItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void ObjectQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void QuestCreatureQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t GetQuestSuggestedPlayers(int32_t index);
int32_t GetQuestSortId(uint32_t index);
const char* GetQuestTitleText(int32_t questId);
const char* GetQuestTitle(int32_t index);
const char* GetQuestTag(int32_t index);
int32_t GetQuestLevel(int32_t questId);
uint32_t IsDailyQuest(int32_t index);
int32_t CalculateExperience(int32_t questId, int32_t level);
int32_t GetNumQuestLeaderBoards(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);
int32_t CountQuestItemDropsNeeded(int32_t questId, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);

class CGQuestLog {
    public:
    static int32_t GetQuestLogSlot(int32_t index);
    static int32_t IsOnQuest(int32_t questId);
    static int32_t FindOrFreePOISlot(int32_t questId);
    static int32_t GetQuestLeaderBoardByID(int32_t questId, QuestLogLeaderBoardParams* params);
    static int32_t GetQuestLeaderBoardForObject(int32_t questId, int32_t objectId, QuestLogLeaderBoardParams* params);
    static int32_t m_sortTypes[25];
    static uint32_t m_numSortTypes;
    static int32_t m_questResetTime;
    static uint8_t m_trackedQuestsLoaded;
    static void FilterAndSortQuests();
    static void LoadTrackedQuests();
    static bool HasQuestPOI(int32_t questId);
    static int32_t IsSelectedQuestExpired();
    static void RemoveQuestWatch(int32_t questId);
    static void ShiftQuestWatches(uint32_t from, uint32_t to);
    static CGItem_C* GetQuestSpecialItem(uint32_t index);
    static void UpdatePOIIcons();
    static int32_t GetQuestWorldMapAreaID(int32_t questId, int32_t* floor);
    static int32_t GetPOIQuestIDByVisibleIndex(int32_t index);
    static void AbandonSelectedQuest();
    static int32_t AddQuestWatch(int32_t questId, int32_t duration);
    static bool SortQuestWatches();
    static void CollapseHeader(uint32_t index, int32_t collapse);
    static int32_t GetQuestIndex(int32_t questId);
    static int32_t IsQuestFailed(uint32_t index);
    static bool HasNoObjectives(uint32_t index);
    static QuestLogEntry m_quests[50];
    static QuestPOIInfo m_questPOIs[25];
    static int32_t m_abandonQuest;
    static uint32_t m_numQuestWatches;
    static int32_t m_timeOffset;
    static int32_t m_nextQuestUpdate;
    static int32_t GetSelectedLogEntry();
    static void SetSelectedQuest(int32_t index);
    static int32_t SetQuestExpired(uint32_t index);
    static void UpdateQuestPOIFromUserServer();
    static QuestWatch m_questWatches[25];
    static uint32_t m_numShownQuests;
    static void Update(int32_t a1);
    static void EnterWorld();
    static int32_t m_numQuest;
    static int32_t m_selectedQuest;
    static float m_questIconOverlapDist;
    static float m_questIconOverlapPushDist;

    static int32_t GetQuestGreenRange();

    static void InitializeGame();
    static int32_t GetQuestAt(int32_t index);
    static int32_t GetQuestLeaderBoardForItem(int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params);
    static int32_t IsQuestCompleteEx(uint32_t index, int32_t a2);
    static void UpdateWatchTimers();
    static void UpdateQuestItem(int32_t itemId);
    static void HandlePOIQueryResponse(CDataStore* msg);
};

int32_t Script_GetQuestLogItemDrop(lua_State* L);
int32_t Script_GetQuestPOILeaderBoard(lua_State* L);
int32_t Script_QuestMapUpdateAllQuests(lua_State* L);
int32_t Script_QuestPOIUpdateTexture(lua_State* L);
int32_t Script_QuestPOIGetIconInfo(lua_State* L);
void QuestLogRegisterScriptFunctions();
void AddQuestPointDisplay();
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t Packet_SMSG_QUERY_TIME_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t UpdateQuestPOIHandler(const char* command, const char* arguments);


#endif
