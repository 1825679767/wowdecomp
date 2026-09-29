#include "QuestFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEFD8
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseQuest", &Script_CloseQuest },
    { "GetTitleText", &Script_GetTitleText },
    { "GetGreetingText", &Script_GetGreetingText },
    { "GetQuestText", &Script_GetQuestText },
    { "GetObjectiveText", &Script_GetObjectiveText },
    { "GetProgressText", &Script_GetProgressText },
    { "GetRewardText", &Script_GetRewardText },
    { "GetNumAvailableQuests", &Script_GetNumAvailableQuests },
    { "GetNumActiveQuests", &Script_GetNumActiveQuests },
    { "GetAvailableTitle", &Script_GetAvailableTitle },
    { "GetActiveTitle", &Script_GetActiveTitle },
    { "GetAvailableLevel", &Script_GetAvailableLevel },
    { "GetActiveLevel", &Script_GetActiveLevel },
    { "IsAvailableQuestTrivial", &Script_IsAvailableQuestTrivial },
    { "IsActiveQuestTrivial", &Script_IsActiveQuestTrivial },
    { "SelectAvailableQuest", &Script_SelectAvailableQuest },
    { "SelectActiveQuest", &Script_SelectActiveQuest },
    { "AcceptQuest", &Script_AcceptQuest },
    { "DeclineQuest", &Script_DeclineQuest },
    { "IsQuestCompletable", &Script_IsQuestCompletable },
    { "CompleteQuest", &Script_CompleteQuest },
    { "GetQuestReward", &Script_GetQuestReward },
    { "GetRewardMoney", &Script_GetRewardMoney },
    { "GetRewardXP", &Script_GetRewardXP },
    { "GetRewardHonor", &Script_GetRewardHonor },
    { "GetRewardSpell", &Script_GetRewardSpell },
    { "GetQuestMoneyToGet", &Script_GetQuestMoneyToGet },
    { "GetNumQuestRewards", &Script_GetNumQuestRewards },
    { "GetNumQuestChoices", &Script_GetNumQuestChoices },
    { "GetNumQuestItems", &Script_GetNumQuestItems },
    { "GetQuestItemInfo", &Script_GetQuestItemInfo },
    { "GetQuestItemLink", &Script_GetQuestItemLink },
    { "GetQuestSpellLink", &Script_GetQuestSpellLink },
    { "QuestChooseRewardError", &Script_QuestChooseRewardError },
    { "ConfirmAcceptQuest", &Script_ConfirmAcceptQuest },
    { "GetQuestBackgroundMaterial", &Script_GetQuestBackgroundMaterial },
    { "GetSuggestedGroupNum", &Script_GetSuggestedGroupNum },
    { "QuestFlagsPVP", &Script_QuestFlagsPVP },
    { "QuestGetAutoAccept", &Script_QuestGetAutoAccept },
    { "GetDailyQuestsCompleted", &Script_GetDailyQuestsCompleted },
    { "GetMaxDailyQuests", &Script_GetMaxDailyQuests },
    { "GetRewardTitle", &Script_GetRewardTitle },
    { "GetRewardTalents", &Script_GetRewardTalents },
    { "GetRewardArenaPoints", &Script_GetRewardArenaPoints },
    { "GetAvailableQuestInfo", &Script_GetAvailableQuestInfo },
    { "QuestIsDaily", &Script_QuestIsDaily },
    { "QuestIsWeekly", &Script_QuestIsWeekly },
};

// VERIFY: 9a990fcd auto
// OFFSET: 0x0058C550
void QuestInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
