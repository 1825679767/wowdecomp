#include "AchievementInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <ctime>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"

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

// VERIFY: 1479ef2e auto
// OFFSET: 0x005B0DE0
bool CGAchievementInfo::IsCriteriaStat(int32_t criteriaId) {
    Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(criteriaId);

    if (!criteria) {
        return false;
    }

    AchievementRec* achievement = g_achievementDB.GetRecord(criteria->m_achievementID);

    if (!achievement) {
        return false;
    }

    Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecord(achievement->m_category);

    if (!category) {
        return false;
    }

    if (category->m_id == 1) {
        return true;
    }

    category = g_achievement_CategoryDB.GetRecord(category->m_parent);

    if (!category) {
        return false;
    }

    while (category->m_id != 1) {
        category = g_achievement_CategoryDB.GetRecord(category->m_parent);

        if (!category) {
            return false;
        }
    }

    return true;
}

// VERIFY: 06c77f2b manual
// OFFSET: 0x005B2520
void CGAchievementInfo::UpdateCriteria(int32_t id, uint64_t counter, uint32_t flags, WowTime date, WOWGUID player, uint32_t timer1, uint32_t timer2) {
    CLIENT_ACHIEVEMENT_CRITERIA* criteria = nullptr;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_unkACFD98.Head(); node; node = s_unkACFD98.Next(node)) {
        if (node->m_id == id) {
            criteria = node;
            break;
        }
    }

    if (!criteria) {
        criteria = s_unkACFD98.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENT_CRITERIA* position = s_unkACFD98.Head();

        while (position && position->m_date > date) {
            position = s_unkACFD98.Next(position);
        }

        s_unkACFD98.LinkNode(criteria, 2, position);
    }

    criteria->m_counter = counter;
    criteria->m_id = id;
    criteria->m_flags = flags;
    criteria->m_date = date;
    criteria->m_player = player;
    criteria->m_timer1 = timer1;
    criteria->m_timer2 = timer2;

    if (flags & 1) {
        s_unkACFD98.UnlinkNode(criteria);
        delete criteria;
        return;
    }

    if (!CGAchievementInfo::IsCriteriaStat(id)) {
        return;
    }

    CLIENT_ACHIEVEMENT_CRITERIA* recent = s_unkACFDA4.NewNode(0, 0, 0);
    recent->m_id = criteria->m_id;
    recent->m_counter = criteria->m_counter;
    recent->m_flags = criteria->m_flags;
    recent->m_date = criteria->m_date;
    recent->m_player = criteria->m_player;
    recent->m_timer1 = criteria->m_timer1;

    Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(recent->m_id);
    CLIENT_ACHIEVEMENT_CRITERIA* node = s_unkACFDA4.Head();
    s_unkACFDA4.LinkNode(recent, 2, node);

    for (int32_t count = 0; node; ) {
        Achievement_CriteriaRec* nodeRec = g_achievement_CriteriaDB.GetRecord(node->m_id);

        if (nodeRec) {
            if (node->m_id == recent->m_id || nodeRec->m_achievementID == rec->m_achievementID) {
                s_unkACFDA4.UnlinkNode(node);
                delete node;
                node = nullptr;
            } else {
                node = s_unkACFDA4.Next(node);
            }
        }

        if (++count >= 5) {
            if (node) {
                s_unkACFDA4.UnlinkNode(node);
                delete node;
            }

            return;
        }
    }
}

// VERIFY: b31d5df0 manual
// OFFSET: 0x005B28D0
void CGAchievementInfo::UpdateAchievement(int32_t id, WowTime date) {
    CLIENT_ACHIEVEMENTS* achievement = nullptr;

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); node; node = s_completedAchievements.Next(node)) {
        if (node->m_id == id) {
            achievement = node;
            break;
        }
    }

    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (!rec) {
        return;
    }

    if (!achievement) {
        achievement = s_completedAchievements.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENTS* position = s_completedAchievements.Head();
        bool found = false;

        while (position && position->m_date >= date && !found) {
            if (position->m_date == date) {
                while (position && position->m_date >= date && !found) {
                    AchievementRec* other = g_achievementDB.GetRecord(position->m_id);

                    if (other && rec->m_uiOrder > other->m_uiOrder) {
                        found = true;
                    } else {
                        position = s_completedAchievements.Next(position);
                    }
                }
            } else {
                position = s_completedAchievements.Next(position);
            }
        }

        s_completedAchievements.LinkNode(achievement, 2, position);
    }

    achievement->m_id = id;
    achievement->m_date = date;
}

// VERIFY: 27756e9f manual
// OFFSET: 0x005B32F0
int32_t Packet_SMSG_ALL_ACHIEVEMENT_DATA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGAchievementInfo::m_showAchievementUI = 1;
    WowTime date;
    int32_t now = static_cast<int32_t>(::time(nullptr));
    int32_t id;
    msg->Get(reinterpret_cast<uint32_t&>(id));

    while (id >= 0) {
        *msg >> date;
        CGAchievementInfo::UpdateAchievement(id, date);
        msg->Get(reinterpret_cast<uint32_t&>(id));
    }

    msg->Get(reinterpret_cast<uint32_t&>(id));

    while (id >= 0) {
        SmartGUID counter;
        SmartGUID player;
        uint32_t flags;
        uint32_t timer1;
        uint32_t timer2;
        *msg >> counter;
        *msg >> player;
        msg->Get(flags);
        *msg >> date;
        msg->Get(timer1);
        msg->Get(timer2);
        CGAchievementInfo::UpdateCriteria(id, counter.m_guid, flags, date, player, now - timer1, now - timer2);
        Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(id);

        if (rec && rec->m_timerTime) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d%d%d%d", rec->m_achievementID, rec->m_id, timer1, rec->m_timerTime);
        }

        msg->Get(reinterpret_cast<uint32_t&>(id));
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, nullptr);
    FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_LIST, nullptr);
    return 1;
}

// VERIFY: e308b3c1 manual
// OFFSET: 0x005B23E0
int32_t AddTrackedAchievement(int32_t id) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); node; node = s_trackedAchievements.Next(node)) {
        count++;

        if (node->m_id == id) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", id);
            return 1;
        }

        if (count == 10) {
            return 0;
        }
    }

    CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.NewNode(2, 0, 0);
    node->m_id = id;
    FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", id);
    return 1;
}

// VERIFY: 9a464c17 manual
// OFFSET: 0x005B3840
void CGAchievementInfo::LoadTrackedAchievements() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedAchievements");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t id = list.GetNextValue(); id; id = list.GetNextValue()) {
        AddTrackedAchievement(id);
    }

    if (list.m_state != 1) {
        char value[256];
        value[0] = 'v';
        value[1] = 1;
        value[list.m_pos] = '\0';
        list.m_cvar->Set(value, true, false, false, true);
    }
}

// VERIFY: 628f101d manual
// OFFSET: 0x005B4F00
void CGAchievementInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_ACHIEVEMENT_EARNED);
    ClientServices::ClearMessageHandler(SMSG_CRITERIA_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_RESPOND_INSPECT_ACHIEVEMENTS);
    ClientServices::ClearMessageHandler(SMSG_ALL_ACHIEVEMENT_DATA);
    ClientServices::ClearMessageHandler(SMSG_ACHIEVEMENT_DELETED);
    ClientServices::ClearMessageHandler(SMSG_CRITERIA_DELETED);
    ClientServices::ClearMessageHandler(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE);
    s_unkC1F450 = 0;
    s_unkACFD98.Clear();
    s_unkACFDA4.Clear();
    s_completedAchievements.Clear();
    s_unkACFDBC.Clear();
    s_unkACFDC8.Clear();
    s_unkACFDD4.Clear();
    s_trackedAchievements.Clear();
    s_completedQuests.Clear();

    for (uint32_t i = 0; i < 124; i++) {
        ACHIEVEMENTCRITERIALIST* list = s_criteriaByType[i].m_fulllist.Head();

        while (list) {
            ACHIEVEMENTCRITERIALIST* next = s_criteriaByType[i].Next(list);
            list->m_criteria.Clear();
            s_criteriaByType[i].Delete(list);
            list = next;
        }
    }

    ACHIEVEMENT* achievement = s_achievements.m_fulllist.Head();

    while (achievement) {
        ACHIEVEMENT* next = s_achievements.Next(achievement);
        achievement->m_criteria.Clear();
        s_achievements.Delete(achievement);
        achievement = next;
    }
}

// VERIFY: f42c8dbe manual
// OFFSET: 0x005B1FA0
void CGAchievementInfo::SaveTrackedAchievements() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedAchievements");
    list.m_pos = 2;
    list.m_state = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); node; node = s_trackedAchievements.Next(node)) {
        list.AddNextValue(node->m_id);
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}
