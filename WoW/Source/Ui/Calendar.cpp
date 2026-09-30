#include "Calendar.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/HolidayNamesRec.h"
#include "WoW/Source/DB/Rec/HolidaysRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

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
