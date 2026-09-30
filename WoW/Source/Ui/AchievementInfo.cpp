#include "AchievementInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00ACFD98
static TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_unkACFD98;

// STATIC: 0x00ACFDA4
static TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_unkACFDA4;

// STATIC: 0x00ACFDB0
TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_completedAchievements;

// STATIC: 0x00ACFDBC
static TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_unkACFDBC;

// STATIC: 0x00ACFDC8
static TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_unkACFDC8;

// STATIC: 0x00ACFDD4
static TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_unkACFDD4;

// STATIC: 0x00ACFDE0
TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_trackedAchievements;

// STATIC: 0x00ACFDEC
static TSList<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>> s_completedQuests;

// STATIC: 0x00C1F450
static int32_t s_unkC1F450;

// STATIC: 0x00C1F460
static TSHashTable<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE> s_criteriaByType[124];

// STATIC: 0x00C207C0
static TSHashTable<ACHIEVEMENT, HASHKEY_NONE> s_achievements;

// STATIC: 0x00C1F454
int32_t CGAchievementInfo::m_showAchievementUI;
// STATIC: 0x00C1F458
WOWGUID CGAchievementInfo::m_inspect;


// VERIFY: 210ec3af manual
// OFFSET: 0x005B6DF0
void CGAchievementInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ACHIEVEMENT_EARNED, Packet_SMSG_ACHIEVEMENT_EARNED, nullptr);
    ClientServices::SetMessageHandler(SMSG_CRITERIA_UPDATE, Packet_SMSG_CRITERIA_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESPOND_INSPECT_ACHIEVEMENTS, Packet_SMSG_RESPOND_INSPECT_ACHIEVEMENTS, nullptr);
    ClientServices::SetMessageHandler(SMSG_ALL_ACHIEVEMENT_DATA, Packet_SMSG_ALL_ACHIEVEMENT_DATA, nullptr);
    ClientServices::SetMessageHandler(SMSG_CRITERIA_DELETED, Packet_SMSG_CRITERIA_DELETED, nullptr);
    ClientServices::SetMessageHandler(SMSG_ACHIEVEMENT_DELETED, Packet_SMSG_ACHIEVEMENT_DELETED, nullptr);
    ClientServices::SetMessageHandler(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE, Packet_SMSG_QUERY_QUESTS_COMPLETED_RESPONSE, nullptr);

    s_unkC1F450 = 1;
    CGAchievementInfo::m_showAchievementUI = 0;
    s_unkACFD98.Clear();
    s_unkACFDA4.Clear();
    s_completedAchievements.Clear();
    CGAchievementInfo::m_inspect.m_guid = 0;
    s_unkACFDBC.Clear();
    s_unkACFDC8.Clear();
    s_unkACFDD4.Clear();
    s_trackedAchievements.Clear();
    s_completedQuests.Clear();

    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* rec = i < 0 || i >= g_achievementDB.m_numRecords ? nullptr : &g_achievementDB.m_records[i];
        int32_t id = rec->m_id;

        ACHIEVEMENT* achievement = s_achievements.New(id, HASHKEY_NONE(), 0, 0);
        achievement->m_rec = rec;
    }

    for (int32_t i = 0; i < g_achievement_CriteriaDB.m_numRecords; i++) {
        Achievement_CriteriaRec* rec = i < 0 || i >= g_achievement_CriteriaDB.m_numRecords ? nullptr : &g_achievement_CriteriaDB.m_records[i];

        if (!rec || !g_achievementDB.GetRecord(rec->m_achievementID) || static_cast<uint32_t>(rec->m_type) > 123) {
            continue;
        }

        ACHIEVEMENTCRITERIALIST* list = s_criteriaByType[rec->m_type].Ptr(rec->m_assetID, HASHKEY_NONE());

        if (!list) {
            list = s_criteriaByType[rec->m_type].New(rec->m_assetID, HASHKEY_NONE(), 0, 0);
        }

        list->m_criteria.NewNode(2, 0, 0)->m_criteria = rec;

        ACHIEVEMENT* achievement = s_achievements.Ptr(rec->m_achievementID, HASHKEY_NONE());

        if (achievement) {
            achievement->m_criteria.NewNode(2, 0, 0)->m_criteria = rec;
        }
    }
}
