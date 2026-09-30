#ifndef WOW_SOURCE_UI_ACHIEVEMENTINFO_H
#define WOW_SOURCE_UI_ACHIEVEMENTINFO_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"

class AchievementRec;
class Achievement_CriteriaRec;
class CDataStore;
struct lua_State;

struct CLIENT_ACHIEVEMENT_CRITERIA : public TSLinkedNode<CLIENT_ACHIEVEMENT_CRITERIA> {
};

struct CLIENT_ACHIEVEMENTS : public TSLinkedNode<CLIENT_ACHIEVEMENTS> {
    /* 0008 */ int32_t m_id;
};

struct COMPLETED_QUESTS : public TSLinkedNode<COMPLETED_QUESTS> {
};

struct SPECIFIC_CRITERIA : public TSLinkedNode<SPECIFIC_CRITERIA> {
    /* 0008 */ Achievement_CriteriaRec* m_criteria;
};

struct ACHIEVEMENTCRITERIALIST : public TSHashObject<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE> {
    /* 0018 */ TSList<SPECIFIC_CRITERIA, TSGetLink<SPECIFIC_CRITERIA>> m_criteria;
};

struct ACHIEVEMENT : public TSHashObject<ACHIEVEMENT, HASHKEY_NONE> {
    /* 0018 */ AchievementRec* m_rec;
    /* 001C */ TSList<SPECIFIC_CRITERIA, TSGetLink<SPECIFIC_CRITERIA>> m_criteria;
};

int32_t Packet_SMSG_ACHIEVEMENT_EARNED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CRITERIA_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_RESPOND_INSPECT_ACHIEVEMENTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ALL_ACHIEVEMENT_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CRITERIA_DELETED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ACHIEVEMENT_DELETED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_QUERY_QUESTS_COMPLETED_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGAchievementInfo {
    public:
    static void InitializeGame();
    static int32_t m_showAchievementUI;
    static WOWGUID m_inspect;
};

int32_t Script_GetCategoryList(lua_State* L);
int32_t Script_GetStatisticsCategoryList(lua_State* L);
int32_t Script_GetCategoryInfo(lua_State* L);
int32_t Script_GetCategoryNumAchievements(lua_State* L);
int32_t Script_GetComparisonCategoryNumAchievements(lua_State* L);
int32_t Script_GetAchievementInfo(lua_State* L);
int32_t Script_GetAchievementNumCriteria(lua_State* L);
int32_t Script_GetAchievementCriteriaInfo(lua_State* L);
int32_t Script_ClearAchievementComparisonUnit(lua_State* L);
int32_t Script_GetAchievementComparisonInfo(lua_State* L);
int32_t Script_GetNextAchievement(lua_State* L);
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
int32_t Script_AddTrackedAchievement(lua_State* L);
int32_t Script_RemoveTrackedAchievement(lua_State* L);
int32_t Script_IsTrackedAchievement(lua_State* L);
int32_t Script_GetNumTrackedAchievements(lua_State* L);
int32_t Script_GetQuestsCompleted(lua_State* L);
void AchievementInfoRegisterScriptFunctions();

extern TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_completedAchievements;
extern TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_trackedAchievements;

#endif
