#include "Calendar.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/HolidayNamesRec.h"
#include "WoW/Source/DB/Rec/HolidaysRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"

// STATIC: 0x00AD0340
static TSList<CalendarPendingInvite, TSGetLink<CalendarPendingInvite> > s_pendingInvites;

// STATIC: 0x00AD0350
static CalendarEventClipboard s_eventClipboard;

// STATIC: 0x00C20F60
static TSFixedArray<int32_t> s_eventTextures[2];

// STATIC: 0x00C207EC
CGCalendarEvent* CGCalendar::m_curEvent;

// STATIC: 0x00C207F0
uint8_t CGCalendar::m_initialize;

// STATIC: 0x00C207F1
uint8_t CGCalendar::m_openedCalendar;

// STATIC: 0x00C207F2
uint8_t CGCalendar::unk_C207F2;

// STATIC: 0x00C207F4
int32_t CGCalendar::m_numInitialPending;

// STATIC: 0x00C207F3
uint8_t CGCalendar::m_actionPending;

// VERIFY: 6a081deb auto
// OFFSET: 0x005C4240
void CGCalendar::InitializeGame() {
    CGCalendar::m_curEvent = nullptr;
    CGCalendar::m_initialize = 1;
    CGCalendar::m_openedCalendar = 0;
    CGCalendar::unk_C207F2 = 0;
    CGCalendar::m_numInitialPending = 0;

    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_CALENDAR, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_EVENT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_STATUS, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_FILTER_GUILD, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_ARENA_TEAM, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_ADDED, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_REMOVED_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_UPDATED_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_NUM_PENDING, PH_CALENDAR_GROUP, nullptr);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_CLEAR_PENDING_ACTION, PH_CALENDAR_GROUP, nullptr);
}

// VERIFY: d8756e5f auto
// OFFSET: 0x005B9430
const char* GetHolidayName(int32_t holidayId) {
    HolidaysRec* holiday = g_holidaysDB.GetRecord(holidayId);

    if (!holiday) {
        return nullptr;
    }

    HolidayNamesRec* name = g_holidayNamesDB.GetRecord(holiday->m_holidayNameID);
    return name ? name->m_name : nullptr;
}

// VERIFY: 0d10e0e8 auto
// OFFSET: 0x005B8E50
static int32_t QSortTextures(const void* a, const void* b) {
    LfgDungeonsRec* recA = g_lfgDungeonsDB.GetRecord(*static_cast<const int32_t*>(a));
    LfgDungeonsRec* recB = g_lfgDungeonsDB.GetRecord(*static_cast<const int32_t*>(b));
    int32_t result = recB->m_expansionLevel - recA->m_expansionLevel;

    if (!result) {
        return SStrSortUTF8I(recA->m_name, recB->m_name, STORM_MAX_STR);
    }

    return result;
}

// VERIFY: de197260 manual
// OFFSET: 0x005BBBB0
void CGCalendar::QueryPendingInvites() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_GET_NUM_PENDING));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 9ea81fd7 manual
// OFFSET: 0x005BE2A0
void CGCalendar::EnterWorld() {
    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);

    if (!CGCalendar::m_initialize) {
        return;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    CGCalendar::QueryPendingInvites();

    uint32_t counts[2] = { 0, 0 };
    int32_t numRecords = g_lfgDungeonsDB.m_numRecords;

    for (int32_t i = 0; i < numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        if (rec->m_faction >= 0) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);
            int32_t group = (faction->m_factionGroup & 0x4) ? 0 : ((faction->m_factionGroup & 0x2) ? 1 : -1);

            if (group != rec->m_faction) {
                continue;
            }
        }

        if (rec->m_typeID == 1) {
            counts[0]++;
        } else if (rec->m_typeID == 2) {
            counts[1]++;
        }
    }

    for (uint32_t i = 0; i < 2; i++) {
        s_eventTextures[i].SetCount(counts[i]);
    }

    counts[0] = 0;
    counts[1] = 0;

    for (int32_t i = 0; i < numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        if (rec->m_faction >= 0) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);
            int32_t group = (faction->m_factionGroup & 0x4) ? 0 : ((faction->m_factionGroup & 0x2) ? 1 : -1);

            if (group != rec->m_faction) {
                continue;
            }
        }

        if (rec->m_typeID == 1) {
            s_eventTextures[0].m_data[counts[0]++] = rec->m_id;
        } else if (rec->m_typeID == 2) {
            s_eventTextures[1].m_data[counts[1]++] = rec->m_id;
        }
    }

    qsort(s_eventTextures[0].m_data, counts[0], sizeof(int32_t), QSortTextures);
    qsort(s_eventTextures[1].m_data, counts[1], sizeof(int32_t), QSortTextures);
    CGCalendar::m_initialize = 0;
}

// VERIFY: 963a0c94 manual
// OFFSET: 0x005BEC70
uint32_t CGCalendar::CountPendingInvites() {
    uint32_t count = 0;

    for (CalendarPendingInvite* invite = s_pendingInvites.Head(); invite; invite = s_pendingInvites.Next(invite)) {
        if (invite->m_inviterGuid && !FriendList::s_friendList->IsIgnored(invite->m_inviterGuid, false)) {
            count++;
        }
    }

    return count > static_cast<uint32_t>(CGCalendar::m_numInitialPending) ? count : CGCalendar::m_numInitialPending;
}
