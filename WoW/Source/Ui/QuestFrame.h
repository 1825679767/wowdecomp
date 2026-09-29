#ifndef WOW_SOURCE_UI_QUESTFRAME_H
#define WOW_SOURCE_UI_QUESTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGQuestInfo {
    public:
    static void EnterWorld();
    static WOWGUID m_npc;
    static WOWGUID m_npcInform;

    static void ConfirmAcceptQuest(int32_t questId, const char* title, const WOWGUID* guid);
};
struct lua_State;

int32_t Script_CloseQuest(lua_State* L);
int32_t Script_GetTitleText(lua_State* L);
int32_t Script_GetGreetingText(lua_State* L);
int32_t Script_GetQuestText(lua_State* L);
int32_t Script_GetObjectiveText(lua_State* L);
int32_t Script_GetProgressText(lua_State* L);
int32_t Script_GetRewardText(lua_State* L);
int32_t Script_GetNumAvailableQuests(lua_State* L);
int32_t Script_GetNumActiveQuests(lua_State* L);
int32_t Script_GetAvailableTitle(lua_State* L);
int32_t Script_GetActiveTitle(lua_State* L);
int32_t Script_GetAvailableLevel(lua_State* L);
int32_t Script_GetActiveLevel(lua_State* L);
int32_t Script_IsAvailableQuestTrivial(lua_State* L);
int32_t Script_IsActiveQuestTrivial(lua_State* L);
int32_t Script_SelectAvailableQuest(lua_State* L);
int32_t Script_SelectActiveQuest(lua_State* L);
int32_t Script_AcceptQuest(lua_State* L);
int32_t Script_DeclineQuest(lua_State* L);
int32_t Script_IsQuestCompletable(lua_State* L);
int32_t Script_CompleteQuest(lua_State* L);
int32_t Script_GetQuestReward(lua_State* L);
int32_t Script_GetRewardMoney(lua_State* L);
int32_t Script_GetRewardXP(lua_State* L);
int32_t Script_GetRewardHonor(lua_State* L);
int32_t Script_GetRewardSpell(lua_State* L);
int32_t Script_GetQuestMoneyToGet(lua_State* L);
int32_t Script_GetNumQuestRewards(lua_State* L);
int32_t Script_GetNumQuestChoices(lua_State* L);
int32_t Script_GetNumQuestItems(lua_State* L);
int32_t Script_GetQuestItemInfo(lua_State* L);
int32_t Script_GetQuestItemLink(lua_State* L);
int32_t Script_GetQuestSpellLink(lua_State* L);
int32_t Script_QuestChooseRewardError(lua_State* L);
int32_t Script_ConfirmAcceptQuest(lua_State* L);
int32_t Script_GetQuestBackgroundMaterial(lua_State* L);
int32_t Script_GetSuggestedGroupNum(lua_State* L);
int32_t Script_QuestFlagsPVP(lua_State* L);
int32_t Script_QuestGetAutoAccept(lua_State* L);
int32_t Script_GetDailyQuestsCompleted(lua_State* L);
int32_t Script_GetMaxDailyQuests(lua_State* L);
int32_t Script_GetRewardTitle(lua_State* L);
int32_t Script_GetRewardTalents(lua_State* L);
int32_t Script_GetRewardArenaPoints(lua_State* L);
int32_t Script_GetAvailableQuestInfo(lua_State* L);
int32_t Script_QuestIsDaily(lua_State* L);
int32_t Script_QuestIsWeekly(lua_State* L);
void QuestInfoRegisterScriptFunctions();

#endif
