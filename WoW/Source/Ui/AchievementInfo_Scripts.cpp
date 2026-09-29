#include "AchievementInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACFC70
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetCategoryList", &Script_GetCategoryList },
    { "GetStatisticsCategoryList", &Script_GetStatisticsCategoryList },
    { "GetCategoryInfo", &Script_GetCategoryInfo },
    { "GetCategoryNumAchievements", &Script_GetCategoryNumAchievements },
    { "GetComparisonCategoryNumAchievements", &Script_GetComparisonCategoryNumAchievements },
    { "GetAchievementInfo", &Script_GetAchievementInfo },
    { "GetAchievementNumRewards", &Script_GetAchievementNumRewards },
    { "GetAchievementReward", &Script_GetAchievementReward },
    { "GetAchievementNumCriteria", &Script_GetAchievementNumCriteria },
    { "GetAchievementCriteriaInfo", &Script_GetAchievementCriteriaInfo },
    { "SetAchievementComparisonUnit", &Script_SetAchievementComparisonUnit },
    { "ClearAchievementComparisonUnit", &Script_ClearAchievementComparisonUnit },
    { "GetAchievementComparisonInfo", &Script_GetAchievementComparisonInfo },
    { "GetPreviousAchievement", &Script_GetPreviousAchievement },
    { "GetNextAchievement", &Script_GetNextAchievement },
    { "GetAchievementCategory", &Script_GetAchievementCategory },
    { "GetAchievementLink", &Script_GetAchievementLink },
    { "GetNumCompletedAchievements", &Script_GetNumCompletedAchievements },
    { "GetNumComparisonCompletedAchievements", &Script_GetNumComparisonCompletedAchievements },
    { "GetLatestCompletedAchievements", &Script_GetLatestCompletedAchievements },
    { "GetLatestUpdatedStats", &Script_GetLatestUpdatedStats },
    { "GetLatestCompletedComparisonAchievements", &Script_GetLatestCompletedComparisonAchievements },
    { "GetLatestUpdatedComparisonStats", &Script_GetLatestUpdatedComparisonStats },
    { "GetTotalAchievementPoints", &Script_GetTotalAchievementPoints },
    { "GetAchievementInfoFromCriteria", &Script_GetAchievementInfoFromCriteria },
    { "GetStatistic", &Script_GetStatistic },
    { "GetComparisonStatistic", &Script_GetComparisonStatistic },
    { "GetComparisonAchievementPoints", &Script_GetComparisonAchievementPoints },
    { "CanShowAchievementUI", &Script_CanShowAchievementUI },
    { "GetTrackedAchievements", &Script_GetTrackedAchievements },
    { "AddTrackedAchievement", &Script_AddTrackedAchievement },
    { "RemoveTrackedAchievement", &Script_RemoveTrackedAchievement },
    { "IsTrackedAchievement", &Script_IsTrackedAchievement },
    { "GetNumTrackedAchievements", &Script_GetNumTrackedAchievements },
    { "HasCompletedAnyAchievement", &Script_HasCompletedAnyAchievement },
    { "QueryQuestsCompleted", &Script_QueryQuestsCompleted },
    { "GetQuestsCompleted", &Script_GetQuestsCompleted },
};

// VERIFY: 5d0272df auto
// OFFSET: 0x005B0D00
void AchievementInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
