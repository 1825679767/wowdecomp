#ifndef WOW_SOURCE_UI_QUESTLOG_H
#define WOW_SOURCE_UI_QUESTLOG_H

#include <cstddef>
#include <cstdint>

class CDataStore;

struct lua_State;

class QuestLogLeaderBoardParams;

class CGQuestLog {
    public:
    static void Update(int32_t a1);
    static void EnterWorld();
    static uint32_t m_numQuest;

    static int32_t GetQuestGreenRange();

    static void InitializeGame();
    static int32_t GetQuestAt(uint32_t index);
    static int32_t GetQuestLeaderBoardForItem(int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params);
    static int32_t IsQuestCompleteEx(uint32_t index, int32_t a2);
    static void UpdateWatchTimers();
    static void UpdateQuestItem(int32_t itemId);
    static void HandlePOIQueryResponse(CDataStore* msg);
};

int32_t Script_GetNumQuestLogEntries(lua_State* L);
int32_t Script_GetQuestLogTitle(lua_State* L);
int32_t Script_SelectQuestLogEntry(lua_State* L);
int32_t Script_GetQuestLogSelection(lua_State* L);
int32_t Script_SetAbandonQuest(lua_State* L);
int32_t Script_GetAbandonQuestName(lua_State* L);
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
int32_t Script_GetQuestLogPushable(lua_State* L);
int32_t Script_QuestLogPushQuest(lua_State* L);
int32_t Script_GetQuestTimers(lua_State* L);
int32_t Script_GetQuestIndexForTimer(lua_State* L);
int32_t Script_CollapseQuestHeader(lua_State* L);
int32_t Script_ExpandQuestHeader(lua_State* L);
int32_t Script_GetQuestGreenRange(lua_State* L);
int32_t Script_GetNumQuestWatches(lua_State* L);
int32_t Script_IsQuestWatched(lua_State* L);
int32_t Script_AddQuestWatch(lua_State* L);
int32_t Script_RemoveQuestWatch(lua_State* L);
int32_t Script_GetQuestIndexForWatch(lua_State* L);
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
int32_t Script_GetNumQuestLogRewardFactions(lua_State* L);
int32_t Script_GetQuestLogRewardFactionInfo(lua_State* L);
int32_t Script_SortQuestWatches(lua_State* L);
int32_t Script_ShiftQuestWatches(lua_State* L);
int32_t Script_GetQuestWatchIndex(lua_State* L);
int32_t Script_QuestMapUpdateAllQuests(lua_State* L);
int32_t Script_GetQuestSortIndex(lua_State* L);
int32_t Script_GetQuestWorldMapAreaID(lua_State* L);
int32_t Script_QuestPOIUpdateTexture(lua_State* L);
int32_t Script_QuestPOIUpdateIcons(lua_State* L);
int32_t Script_QuestPOIGetIconInfo(lua_State* L);
int32_t Script_QuestPOIGetQuestIDByIndex(lua_State* L);
int32_t Script_QuestPOIGetQuestIDByVisibleIndex(lua_State* L);
int32_t Script_GetQuestLogCompletionText(lua_State* L);
int32_t Script_SetPOIIconOverlapDistance(lua_State* L);
int32_t Script_SetPOIIconOverlapPushDistance(lua_State* L);
void QuestLogRegisterScriptFunctions();

int32_t Script_GetQuestLogLeaderBoard(int32_t questId, uint32_t objectId, QuestLogLeaderBoardParams* params);

#endif
