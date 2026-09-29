#include "QuestLog.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD1020
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumQuestLogEntries", &Script_GetNumQuestLogEntries },
    { "GetQuestLogTitle", &Script_GetQuestLogTitle },
    { "SelectQuestLogEntry", &Script_SelectQuestLogEntry },
    { "GetQuestLogSelection", &Script_GetQuestLogSelection },
    { "SetAbandonQuest", &Script_SetAbandonQuest },
    { "GetAbandonQuestName", &Script_GetAbandonQuestName },
    { "GetAbandonQuestItems", &Script_GetAbandonQuestItems },
    { "AbandonQuest", &Script_AbandonQuest },
    { "IsUnitOnQuest", &Script_IsUnitOnQuest },
    { "GetQuestLogQuestText", &Script_GetQuestLogQuestText },
    { "GetNumQuestLeaderBoards", &Script_GetNumQuestLeaderBoards },
    { "GetQuestLogLeaderBoard", &Script_GetQuestLogLeaderBoard },
    { "GetNumQuestItemDrops", &Script_GetNumQuestItemDrops },
    { "GetQuestLogItemDrop", &Script_GetQuestLogItemDrop },
    { "GetQuestPOILeaderBoard", &Script_GetQuestPOILeaderBoard },
    { "GetQuestLogTimeLeft", &Script_GetQuestLogTimeLeft },
    { "IsCurrentQuestFailed", &Script_IsCurrentQuestFailed },
    { "GetNumQuestLogRewards", &Script_GetNumQuestLogRewards },
    { "GetNumQuestLogChoices", &Script_GetNumQuestLogChoices },
    { "GetQuestLogRewardInfo", &Script_GetQuestLogRewardInfo },
    { "GetQuestLogChoiceInfo", &Script_GetQuestLogChoiceInfo },
    { "GetQuestLogItemLink", &Script_GetQuestLogItemLink },
    { "GetQuestLogSpellLink", &Script_GetQuestLogSpellLink },
    { "GetQuestLogRewardMoney", &Script_GetQuestLogRewardMoney },
    { "GetQuestLogRewardXP", &Script_GetQuestLogRewardXP },
    { "GetQuestLogRewardHonor", &Script_GetQuestLogRewardHonor },
    { "GetQuestLogRewardSpell", &Script_GetQuestLogRewardSpell },
    { "GetQuestLogRequiredMoney", &Script_GetQuestLogRequiredMoney },
    { "GetQuestLogPushable", &Script_GetQuestLogPushable },
    { "QuestLogPushQuest", &Script_QuestLogPushQuest },
    { "GetQuestTimers", &Script_GetQuestTimers },
    { "GetQuestIndexForTimer", &Script_GetQuestIndexForTimer },
    { "CollapseQuestHeader", &Script_CollapseQuestHeader },
    { "ExpandQuestHeader", &Script_ExpandQuestHeader },
    { "GetQuestGreenRange", &Script_GetQuestGreenRange },
    { "GetNumQuestWatches", &Script_GetNumQuestWatches },
    { "IsQuestWatched", &Script_IsQuestWatched },
    { "AddQuestWatch", &Script_AddQuestWatch },
    { "RemoveQuestWatch", &Script_RemoveQuestWatch },
    { "GetQuestIndexForWatch", &Script_GetQuestIndexForWatch },
    { "GetQuestLogGroupNum", &Script_GetQuestLogGroupNum },
    { "GetQuestResetTime", &Script_GetQuestResetTime },
    { "GetQuestLink", &Script_GetQuestLink },
    { "GetQuestLogRewardTitle", &Script_GetQuestLogRewardTitle },
    { "GetQuestLogRewardTalents", &Script_GetQuestLogRewardTalents },
    { "GetQuestLogRewardArenaPoints", &Script_GetQuestLogRewardArenaPoints },
    { "GetQuestLogSpecialItemInfo", &Script_GetQuestLogSpecialItemInfo },
    { "GetQuestLogSpecialItemCooldown", &Script_GetQuestLogSpecialItemCooldown },
    { "IsQuestLogSpecialItemInRange", &Script_IsQuestLogSpecialItemInRange },
    { "UseQuestLogSpecialItem", &Script_UseQuestLogSpecialItem },
    { "ProcessQuestLogRewardFactions", &Script_ProcessQuestLogRewardFactions },
    { "GetNumQuestLogRewardFactions", &Script_GetNumQuestLogRewardFactions },
    { "GetQuestLogRewardFactionInfo", &Script_GetQuestLogRewardFactionInfo },
    { "SortQuestWatches", &Script_SortQuestWatches },
    { "ShiftQuestWatches", &Script_ShiftQuestWatches },
    { "GetQuestWatchIndex", &Script_GetQuestWatchIndex },
    { "QuestMapUpdateAllQuests", &Script_QuestMapUpdateAllQuests },
    { "GetQuestSortIndex", &Script_GetQuestSortIndex },
    { "GetQuestWorldMapAreaID", &Script_GetQuestWorldMapAreaID },
    { "QuestPOIUpdateTexture", &Script_QuestPOIUpdateTexture },
    { "QuestPOIUpdateIcons", &Script_QuestPOIUpdateIcons },
    { "QuestPOIGetIconInfo", &Script_QuestPOIGetIconInfo },
    { "QuestPOIGetQuestIDByIndex", &Script_QuestPOIGetQuestIDByIndex },
    { "QuestPOIGetQuestIDByVisibleIndex", &Script_QuestPOIGetQuestIDByVisibleIndex },
    { "GetQuestLogCompletionText", &Script_GetQuestLogCompletionText },
    { "SetPOIIconOverlapDistance", &Script_SetPOIIconOverlapDistance },
    { "SetPOIIconOverlapPushDistance", &Script_SetPOIIconOverlapPushDistance },
};

// VERIFY: 3feca0f3 auto
// OFFSET: 0x005DFD20
void QuestLogRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
