#ifndef WOW_SOURCE_UI_QUESTLOG_H
#define WOW_SOURCE_UI_QUESTLOG_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

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

class CGQuestLog {
    public:
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

int32_t Script_GetQuestLogTitle(lua_State* L);
int32_t Script_GetAbandonQuestItems(lua_State* L);
int32_t Script_AbandonQuest(lua_State* L);
int32_t Script_IsUnitOnQuest(lua_State* L);
int32_t Script_GetQuestLogQuestText(lua_State* L);
int32_t Script_GetNumQuestLeaderBoards(lua_State* L);
int32_t Script_GetQuestLogLeaderBoard(lua_State* L);
int32_t Script_GetNumQuestItemDrops(lua_State* L);
int32_t Script_GetQuestLogItemDrop(lua_State* L);
int32_t Script_GetQuestPOILeaderBoard(lua_State* L);
int32_t Script_GetQuestLogTimeLeft(lua_State* L);
int32_t Script_IsCurrentQuestFailed(lua_State* L);
int32_t Script_GetNumQuestLogRewards(lua_State* L);
int32_t Script_GetNumQuestLogChoices(lua_State* L);
int32_t Script_GetQuestLogRewardInfo(lua_State* L);
int32_t Script_GetQuestLogChoiceInfo(lua_State* L);
int32_t Script_GetQuestLogItemLink(lua_State* L);
int32_t Script_GetQuestLogSpellLink(lua_State* L);
int32_t Script_GetQuestLogRewardMoney(lua_State* L);
int32_t Script_GetQuestLogRewardXP(lua_State* L);
int32_t Script_GetQuestLogRewardHonor(lua_State* L);
int32_t Script_GetQuestLogRewardSpell(lua_State* L);
int32_t Script_GetQuestLogRequiredMoney(lua_State* L);
int32_t Script_QuestLogPushQuest(lua_State* L);
int32_t Script_GetQuestIndexForTimer(lua_State* L);
int32_t Script_CollapseQuestHeader(lua_State* L);
int32_t Script_ExpandQuestHeader(lua_State* L);
int32_t Script_AddQuestWatch(lua_State* L);
int32_t Script_RemoveQuestWatch(lua_State* L);
int32_t Script_GetQuestLogGroupNum(lua_State* L);
int32_t Script_GetQuestResetTime(lua_State* L);
int32_t Script_GetQuestLink(lua_State* L);
int32_t Script_GetQuestLogRewardTitle(lua_State* L);
int32_t Script_GetQuestLogRewardTalents(lua_State* L);
int32_t Script_GetQuestLogRewardArenaPoints(lua_State* L);
int32_t Script_GetQuestLogSpecialItemInfo(lua_State* L);
int32_t Script_GetQuestLogSpecialItemCooldown(lua_State* L);
int32_t Script_IsQuestLogSpecialItemInRange(lua_State* L);
int32_t Script_UseQuestLogSpecialItem(lua_State* L);
int32_t Script_ProcessQuestLogRewardFactions(lua_State* L);
int32_t Script_GetQuestLogRewardFactionInfo(lua_State* L);
int32_t Script_SortQuestWatches(lua_State* L);
int32_t Script_ShiftQuestWatches(lua_State* L);
int32_t Script_QuestMapUpdateAllQuests(lua_State* L);
int32_t Script_GetQuestSortIndex(lua_State* L);
int32_t Script_GetQuestWorldMapAreaID(lua_State* L);
int32_t Script_QuestPOIUpdateTexture(lua_State* L);
int32_t Script_QuestPOIUpdateIcons(lua_State* L);
int32_t Script_QuestPOIGetIconInfo(lua_State* L);
int32_t Script_QuestPOIGetQuestIDByIndex(lua_State* L);
int32_t Script_QuestPOIGetQuestIDByVisibleIndex(lua_State* L);
int32_t Script_GetQuestLogCompletionText(lua_State* L);
void QuestLogRegisterScriptFunctions();
void AddQuestPointDisplay();
void DisplayQuestFailed(int32_t id, const WOWGUID& guid, void* param, bool success);
int32_t Packet_SMSG_QUERY_TIME_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t UpdateQuestPOIHandler(const char* command, const char* arguments);

int32_t Script_GetQuestLogLeaderBoard(int32_t questId, uint32_t objectId, QuestLogLeaderBoardParams* params);

#endif
