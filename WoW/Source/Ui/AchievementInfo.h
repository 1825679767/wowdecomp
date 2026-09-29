#ifndef WOW_SOURCE_UI_ACHIEVEMENTINFO_H
#define WOW_SOURCE_UI_ACHIEVEMENTINFO_H

#include <cstdint>

struct lua_State;

class CGAchievementInfo {
    public:
    static void InitializeGame();
};

int32_t Script_GetCategoryList(lua_State* L);
int32_t Script_GetStatisticsCategoryList(lua_State* L);
int32_t Script_GetCategoryInfo(lua_State* L);
int32_t Script_GetCategoryNumAchievements(lua_State* L);
int32_t Script_GetComparisonCategoryNumAchievements(lua_State* L);
int32_t Script_GetAchievementInfo(lua_State* L);
int32_t Script_GetAchievementNumRewards(lua_State* L);
int32_t Script_GetAchievementReward(lua_State* L);
int32_t Script_GetAchievementNumCriteria(lua_State* L);
int32_t Script_GetAchievementCriteriaInfo(lua_State* L);
int32_t Script_SetAchievementComparisonUnit(lua_State* L);
int32_t Script_ClearAchievementComparisonUnit(lua_State* L);
int32_t Script_GetAchievementComparisonInfo(lua_State* L);
int32_t Script_GetPreviousAchievement(lua_State* L);
int32_t Script_GetNextAchievement(lua_State* L);
int32_t Script_GetAchievementCategory(lua_State* L);
int32_t Script_GetAchievementLink(lua_State* L);
int32_t Script_GetNumCompletedAchievements(lua_State* L);
int32_t Script_GetNumComparisonCompletedAchievements(lua_State* L);
int32_t Script_GetLatestCompletedAchievements(lua_State* L);
int32_t Script_GetLatestUpdatedStats(lua_State* L);
int32_t Script_GetLatestCompletedComparisonAchievements(lua_State* L);
int32_t Script_GetLatestUpdatedComparisonStats(lua_State* L);
int32_t Script_GetTotalAchievementPoints(lua_State* L);
int32_t Script_GetAchievementInfoFromCriteria(lua_State* L);
int32_t Script_GetStatistic(lua_State* L);
int32_t Script_GetComparisonStatistic(lua_State* L);
int32_t Script_GetComparisonAchievementPoints(lua_State* L);
int32_t Script_CanShowAchievementUI(lua_State* L);
int32_t Script_GetTrackedAchievements(lua_State* L);
int32_t Script_AddTrackedAchievement(lua_State* L);
int32_t Script_RemoveTrackedAchievement(lua_State* L);
int32_t Script_IsTrackedAchievement(lua_State* L);
int32_t Script_GetNumTrackedAchievements(lua_State* L);
int32_t Script_HasCompletedAnyAchievement(lua_State* L);
int32_t Script_QueryQuestsCompleted(lua_State* L);
int32_t Script_GetQuestsCompleted(lua_State* L);
void AchievementInfoRegisterScriptFunctions();

#endif
