#include "AchievementInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// VERIFY: e0dcaa80 auto
// OFFSET: 0x005B0CE0
static int32_t Script_CanShowAchievementUI(lua_State* L) {
    lua_pushboolean(L, CGAchievementInfo::m_showAchievementUI);
    return 1;
}

// VERIFY: 2eadd0b6 manual
// OFFSET: 0x005B1BB0
static int32_t Script_QueryQuestsCompleted(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_QUESTS_COMPLETED));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: a12eb50e auto
// OFFSET: 0x005B1690
static int32_t Script_GetPreviousAchievement(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPreviousAchievement(achievementID)");
    }

    AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!rec || !rec->m_supercedes) {
        return 0;
    }

    lua_pushnumber(L, rec->m_supercedes);
    return 1;
}

// VERIFY: 5a4740c3 auto
// OFFSET: 0x005B18E0
static int32_t Script_GetAchievementCategory(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementCategory(achievementID)");
    }

    AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!rec || !rec->m_category || (rec->m_flags & 2)) {
        return 0;
    }

    lua_pushnumber(L, rec->m_category);
    return 1;
}

// VERIFY: 01bbd72e auto
// OFFSET: 0x005B1970
static int32_t Script_GetAchievementNumRewards(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementNumRewards(achievementID)");
    }

    AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (rec && !(rec->m_flags & 2)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 05270d3b auto
// OFFSET: 0x005B1A00
static int32_t Script_GetAchievementReward(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetAchievementNumRewards(achievementID, rewardIndex)");
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    lua_tonumber(L, 2);
    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (rec && !(rec->m_flags & 2)) {
        lua_pushnumber(L, rec->m_points);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 4b6d3002 manual
// OFFSET: 0x005B1AB0
static int32_t Script_SetAchievementComparisonUnit(lua_State* L) {
    WOWGUID guid = WOWGUID();

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: AddAchievementComparisonUnit(unitToken)");
    }

    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);

    if (!guid) {
        return 0;
    }

    CGAchievementInfo::m_inspect = guid;
    lua_pushnumber(L, 1.0);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_INSPECT_ACHIEVEMENTS));
    SmartGUID packed;
    packed.m_guid = guid.m_guid;
    msg << packed;
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 1;
}

// VERIFY: da85ee7f manual
// OFFSET: 0x005B2040
static int32_t Script_GetTrackedAchievements(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); node; node = s_trackedAchievements.Next(node)) {
        count++;
        lua_pushnumber(L, node->m_id);
    }

    return count;
}

// VERIFY: 8a018839 manual
// OFFSET: 0x005B1EC0
static int32_t Script_HasCompletedAnyAchievement(lua_State* L) {
    lua_pushboolean(L, s_completedAchievements.Head() != nullptr);
    return 1;
}

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
