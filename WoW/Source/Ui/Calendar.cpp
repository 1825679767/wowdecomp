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
#include "WoW/Source/Ui/CalendarEvent.h"
#include "WoW/Common/WowTime.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00AD0340
static TSList<CalendarPendingInvite, TSGetLink<CalendarPendingInvite> > s_pendingInvites;

// STATIC: 0x00AD0350
static CalendarEventClipboard s_eventClipboard;

// STATIC: 0x00AD0328
static TSList<CALENDARHOLIDAYNODE, TSGetLink<CALENDARHOLIDAYNODE> > s_holidays;

// STATIC: 0x00AD0334
static TSList<CALENDARRAIDRESETNODE, TSGetLink<CALENDARRAIDRESETNODE> > s_raidResets;

// STATIC: 0x00C20808
static TSGrowableArray<CalendarDayEvent*> s_days[3][31];

// STATIC: 0x00C20DD8
static CalendarEventTables s_eventTables;

// STATIC: 0x00C20F38
static TSHashTable<DYNAMICHOLIDAYHASH, HASHKEY_NONE> s_dynamicHolidays;

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

// STATIC: 0x00ACFFE4
static const char* s_fullDateMonthNames[12] = { "FULLDATE_MONTH_JANUARY", "FULLDATE_MONTH_FEBRUARY", "FULLDATE_MONTH_MARCH", "FULLDATE_MONTH_APRIL", "FULLDATE_MONTH_MAY", "FULLDATE_MONTH_JUNE", "FULLDATE_MONTH_JULY", "FULLDATE_MONTH_AUGUST", "FULLDATE_MONTH_SEPTEMBER", "FULLDATE_MONTH_OCTOBER", "FULLDATE_MONTH_NOVEMBER", "FULLDATE_MONTH_DECEMBER" };

// VERIFY: f2885c19 manual
// OFFSET: 0x005B7E50
size_t CGCalendar::GetDateTimeText(char* buffer, uint32_t size, uint32_t time) {
    WowTime date;
    WowTime::WowDecodeTime(time, &date);
    char dateText[256] = { 0 };

    if (date.m_year >= 0 && date.m_month >= 0 && date.m_monthDay >= 0 && date.m_weekDay >= 0) {
        SStrPrintf(dateText, sizeof(dateText), FrameScript_GetText("FULLDATE", -1, GENDER_NOT_APPLICABLE), FrameScript_GetText(s_weekdayNames[date.m_weekDay], -1, GENDER_NOT_APPLICABLE), FrameScript_GetText(s_fullDateMonthNames[date.m_month], -1, GENDER_NOT_APPLICABLE), date.m_monthDay + 1, date.m_year + 2000, date.m_month + 1);
    }

    char timeText[64] = { 0 };
    int32_t hour = date.m_hour;

    if (hour >= 0) {
        int32_t minute = date.m_minute;

        if (minute >= 0) {
            if (CVar::Lookup("timeMgrUseMilitaryTime")->m_intValue) {
                SStrPrintf(timeText, sizeof(timeText), FrameScript_GetText("TIME_TWENTYFOURHOURS", -1, GENDER_NOT_APPLICABLE), hour, minute);
            } else {
                const char* token = "TIME_TWELVEHOURAM";

                if (!hour) {
                    hour = 12;
                } else if (hour == 12) {
                    token = "TIME_TWELVEHOURPM";
                } else if (hour > 12) {
                    hour -= 12;
                    token = "TIME_TWELVEHOURPM";
                }

                SStrPrintf(timeText, sizeof(timeText), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), hour, minute);
            }
        }
    }

    return SStrPrintf(buffer, size, FrameScript_GetText("FULLDATE_AND_TIME", -1, GENDER_NOT_APPLICABLE), dateText, timeText);
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

// VERIFY: 14fb4767 auto
// OFFSET: 0x005B9960
void CGCalendar::CalendarError(const char* error, uintptr_t arg) {
    char buffer[3000];

    if (arg) {
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(error, -1, GENDER_NOT_APPLICABLE), arg);
    } else {
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(error, -1, GENDER_NOT_APPLICABLE));
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_ERROR, "%s", buffer);
    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);
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

// VERIFY: 6c9627bd manual
// OFFSET: 0x005C3FE0
int32_t PH_CALENDAR_GROUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t clearPending = 1;

    switch (msgId) {
        case SMSG_CALENDAR_SEND_CALENDAR:
            PH_SMSG_CALENDAR_SEND_CALENDAR(msg);
            break;

        case SMSG_CALENDAR_SEND_EVENT:
            CGCalendar::ProcessGetEvent(msg);
            break;

        case SMSG_CALENDAR_FILTER_GUILD:
        case SMSG_CALENDAR_ARENA_TEAM:
            CGCalendar::ProcessInitialInviteList(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE:
            CGCalendar::ProcessInvite(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_INVITE_REMOVED:
            CGCalendar::ProcessInviteRemoved(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_STATUS:
            CGCalendar::ProcessInviteStatus(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_RAID_LOCKOUT_ADDED:
            CGCalendar::ProcessLockoutAdded(msg);
            break;

        case SMSG_CALENDAR_RAID_LOCKOUT_REMOVED:
            CGCalendar::ProcessLockoutRemoved(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE_ALERT:
            CGCalendar::ProcessInviteAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT:
            CGCalendar::ProcessInviteRemovedAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT:
            CGCalendar::ProcessInviteStatusAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_REMOVED_ALERT:
            CGCalendar::ProcessEventRemovedAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_UPDATED_ALERT:
            CGCalendar::ProcessEventUpdatedAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT:
            CGCalendar::ProcessInviteModeratorStatusAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_SEND_NUM_PENDING:
            CGCalendar::UpdatePendingInvites(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE_NOTES:
            CGCalendar::ProcessInviteNotes(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT:
            CGCalendar::ProcessInviteNotesAlert(msg);
            return 1;

        case SMSG_CALENDAR_RAID_LOCKOUT_UPDATED:
            CGCalendar::ProcessLockoutUpdated(msg);
            break;
    }

    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);
    return 1;
}

// VERIFY: d895f606 manual
// OFFSET: 0x005BE220
void CalendarEventTables::Clear() {
    for (uint32_t i = 0; i < 8; i++) {
        this->m_counts[i] = 0;
        this->m_tables[i].InternalClear(0);
    }
}

// VERIFY: 27b54cc9 manual
// OFFSET: 0x005BEAD0
void CGCalendar::ShutdownGame() {
    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    s_eventTables.Clear();
    s_holidays.Clear();
    s_raidResets.Clear();
    s_pendingInvites.Clear();
    s_dynamicHolidays.InternalClear(0);

    for (uint32_t month = 0; month < 3; month++) {
        for (uint32_t day = 0; day < 31; day++) {
            s_days[month][day].m_count = 0;
        }
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (s_eventTextures[i].m_count) {
            s_eventTextures[i].Clear();
        }
    }

    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_CALENDAR);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_EVENT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_STATUS);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_FILTER_GUILD);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_ARENA_TEAM);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_ADDED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_REMOVED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_UPDATED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_NUM_PENDING);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_CLEAR_PENDING_ACTION);
}

// VERIFY: 24ff9070 auto
// OFFSET: 0x005B7CE0
void CGCalendar::UpdatePendingInvites(CDataStore* msg) {
    int32_t previous = CGCalendar::m_numInitialPending;
    msg->Get(reinterpret_cast<uint32_t&>(CGCalendar::m_numInitialPending));

    if (CGCalendar::m_numInitialPending != previous) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, nullptr);
    }
}

// STATIC: 0x00ACFF8C
int32_t CGCalendar::m_absMonth = -1;

// STATIC: 0x00ACFF90
int32_t CGCalendar::m_absYear = 4;

// STATIC: 0x00C20800
int32_t CGCalendar::m_absMonthDays;

// STATIC: 0x00C20F78
CalendarEventSelection CGCalendar::m_selectedEvent;

// STATIC: 0x00C20F90
CalendarEventSelection CGCalendar::m_contextEvent;

// VERIFY: e086b5b0 auto
// OFFSET: 0x005B7A10
int32_t CGCalendar::GetDaysInMonth(int32_t month, int32_t year, int32_t* days, int32_t* weekDay) {
    WowTime date;
    date.m_month = month;
    date.m_monthDay = 0;
    date.m_year = year;
    date.AddDays(0, false);
    *weekDay = date.m_weekDay;
    date.m_month = month + 1;

    if (month == 12) {
        year++;
        date.m_month = 0;
    }

    date.m_year = year;
    date.AddDays(-1, false);
    *days = date.m_monthDay + 1;
    return date.m_monthDay + 1;
}

// VERIFY: a29063fa manual
// OFFSET: 0x005B8EC0
int32_t CGCalendar::GetMonth(int32_t offset, int32_t* month, int32_t* year, int32_t* days, int32_t* weekDay) {
    if (CGCalendar::m_absMonthDays) {
        *month = CGCalendar::m_absMonth;
        *year = CGCalendar::m_absYear;
    } else {
        *month = g_clientGameTime.m_month;
        *year = g_clientGameTime.m_year;
    }

    if (offset) {
        *year += offset / 12;
        *month += offset % 12;

        if (*month >= 12) {
            ++*year;
            *month -= 12;
        } else if (*month < 0) {
            --*year;
            *month += 12;
        }
    }

    int32_t clampedYear = *year;

    if (clampedYear >= 30) {
        clampedYear = 30;
    }

    *year = clampedYear;

    if (clampedYear <= 4) {
        *year = 4;
        int32_t clampedMonth = *month;

        if (clampedMonth <= 10) {
            clampedMonth = 10;
        }

        *month = clampedMonth;
    }

    return CGCalendar::GetDaysInMonth(*month, *year, days, weekDay);
}

// VERIFY: 897b7493 manual
// OFFSET: 0x005B8F70
int32_t CGCalendar::ClampAbsMonthDay(int32_t month, int32_t year, int32_t* outMonth, int32_t* outYear, int32_t* days, int32_t* weekDay) {
    int32_t minYear = g_clientGameTime.m_year - 5;
    int32_t maxYear = minYear + 10;

    if (year < minYear) {
        *outYear = minYear;
    } else if (year <= maxYear) {
        *outYear = year;
    } else {
        *outYear = maxYear;
    }

    int32_t clampedYear = *outYear;

    if (clampedYear >= 30) {
        clampedYear = 30;
    }

    *outYear = clampedYear;

    if (clampedYear < 4) {
        *outYear = 4;
    }

    *outMonth = abs(month % 12);
    return CGCalendar::GetDaysInMonth(*outMonth, *outYear, days, weekDay);
}

// VERIFY: e9369a93 manual
// OFFSET: 0x005B9000
bool CGCalendar::ComputeEventIndex(const CalendarEventSelection* selection, int32_t* monthOffset, int32_t* day, int32_t* index) {
    *monthOffset = 0;
    *day = 0;
    *index = 0;

    if (!selection->m_eventID && !selection->m_holidayID && !selection->m_startTime) {
        return false;
    }

    WowTime date;
    WowTime::WowDecodeTime(selection->m_startTime, &date);
    uint32_t month = 12 * (date.m_year - CGCalendar::m_absYear) - CGCalendar::m_absMonth + date.m_month + 1;

    if (month > 2) {
        return false;
    }

    TSGrowableArray<CalendarDayEvent*>* info = &s_days[month][date.m_monthDay];

    for (uint32_t i = 0; i < info->Count(); i++) {
        CalendarDayEvent* event = info->m_data[i];

        if (!event) {
            continue;
        }

        bool match = false;

        if (selection->m_eventID) {
            match = selection->m_eventID == event->m_eventID;
        }

        if (selection->m_holidayID) {
            match = selection->m_holidayID == event->m_holidayID;

            if (!match) {
                continue;
            }

            if (selection->unk_0014 < 0) {
                match = selection->m_endTime == event->m_endTime;
            }
        }

        if (!match) {
            continue;
        }

        *monthOffset = month - 1;
        *day = date.m_monthDay;
        *index = i;
        return true;
    }

    return false;
}

// STATIC: 0x00C207F8
uint32_t CGCalendar::m_lastInviteTime;

// VERIFY: adfc66bf manual
// OFFSET: 0x005B7A80
bool GameCalendarGetMaxCreateDate(WowTime* date) {
    int32_t years = (g_clientGameTime.m_month + 13) / 12;
    date->m_month = (g_clientGameTime.m_month + 13) % 12;
    date->m_year = years + g_clientGameTime.m_year;
    date->m_monthDay = 0;
    date->AddDays(-1, false);
    int32_t year = date->m_year;

    if (year >= 31) {
        year = 31;
    }

    date->m_year = year;
    return date->SetHourAndMinutes(23u, 59u);
}

// STATIC: 0x00ACFF94
static CVar** s_eventFilters[8] = { nullptr, nullptr, &s_cvCalendarShowLockouts, &s_cvCalendarShowResets, nullptr, &s_cvCalendarShowWeeklyHolidays, &s_cvCalendarShowDarkmoon, &s_cvCalendarShowBattlegrounds };

// VERIFY: 05349707 auto
// OFFSET: 0x005B7820
CalendarDayEvent::CalendarDayEvent() {
    this->m_eventID = 0;
    this->m_startTime = 0;
    this->unk_008C = 0;
    this->m_endTime = 0;
    this->m_holidayID = 0;
    this->m_type = 0;
    this->m_flags = 0;
    this->unk_00A0 = -1;
    this->unk_00A4 = 0;
    this->unk_00A8 = 0;
    this->unk_00B0 = 0;
    this->unk_00B4 = 0;
    this->m_creatorGUID = WOWGUID();
    this->unk_00C0 = 0;
    this->m_sequenceIndex = 0;
    this->m_sequenceLength = 0;
}

// VERIFY: 5c4f6f0e manual
// OFFSET: 0x005B7890
int32_t CalendarDayEvent::GetSequenceType() const {
    if (this->m_sequenceLength <= 1) {
        return 4;
    }

    if (!this->m_sequenceIndex) {
        return 1;
    }

    return (this->m_sequenceIndex != this->m_sequenceLength - 1) + 2;
}

// VERIFY: a55134c1 manual
// OFFSET: 0x005BFAB0
static bool ComputeEventOccurrence(const WowTime* now, const WowTime* rangeStart, const WowTime* rangeEnd, WowTime* outEnd, HolidaysRec* holiday, int32_t index, bool* weekly, int32_t interval, WowTime* outStart) {
    *weekly = false;
    DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(holiday->m_id, HASHKEY_NONE());
    int32_t* durations;
    int32_t* dates;
    int32_t looping;

    if (dynamic) {
        durations = dynamic->m_duration;
        dates = dynamic->m_date;
        looping = dynamic->m_looping;
    } else {
        looping = holiday->m_looping;
        durations = holiday->m_duration;
        dates = holiday->m_date;
    }

    if (index > 0 && !durations[index]) {
        return false;
    }

    int32_t duration = durations[index];

    if (!duration) {
        duration = 24;
    }

    int32_t durationMinutes = 60 * duration;

    for (uint32_t i = 0; i < 26; i++) {
        if (!dates[i]) {
            continue;
        }

        WowTime::WowDecodeTime(dates[i], outStart);
        const WowTime* reference = now;

        if (holiday->m_flags & 0x1) {
            outStart->m_tzHours = g_clientGameTime.m_tzHours;
            outStart->ConvertToLocal(*outStart);
        }

        int32_t days = 0;

        if (outStart->m_weekDay >= 0 && outStart->m_month < 0 && outStart->m_monthDay < 0 && outStart->m_year < 0) {
            reference = rangeStart;
            *weekly = true;
            int32_t diff = outStart->m_weekDay - rangeStart->m_weekDay;
            days = diff + (diff < 0 ? 7 : 0);
        }

        if (outStart->m_minute < 0) {
            outStart->m_minute = reference->m_minute;
        }

        if (outStart->m_hour < 0) {
            outStart->m_hour = reference->m_hour;
        }

        if (outStart->m_monthDay < 0) {
            outStart->m_monthDay = reference->m_monthDay;
        }

        if (outStart->m_month < 0) {
            outStart->m_month = reference->m_month;
        }

        if (outStart->m_year < 0) {
            int32_t year = CGCalendar::m_absYear;

            if (CGCalendar::m_absYear < 0) {
                year = g_clientGameTime.m_year;
                CGCalendar::m_absYear = year;

                if (year < 0) {
                    year = 4;
                    CGCalendar::m_absYear = 4;
                }
            }

            outStart->m_year = year;

            if (!CGCalendar::m_absMonth && outStart->m_month == 11) {
                outStart->m_year--;
            }
        }

        if (days) {
            outStart->AddDays(days, false);
        }

        for (int32_t j = 0; j < index; j++) {
            int32_t previous = durations[j];

            if (!previous) {
                previous = 24;
            }

            outStart->AddHolidayDuration(60 * previous);
        }

        bool after = *rangeEnd < *outStart;

        if (looping) {
            if (after) {
                return false;
            }

            while (true) {
                *outEnd = *outStart;
                outEnd->AddHolidayDuration(durationMinutes);

                if (!(*rangeStart >= *outEnd)) {
                    break;
                }

                outStart->AddHolidayDuration(interval);

                if (*rangeEnd < *outStart) {
                    return false;
                }
            }

            goto found;
        }

        if (!after) {
            *outEnd = *outStart;
            outEnd->AddHolidayDuration(durationMinutes);

            if (!(*outEnd < *rangeStart)) {
                goto found;
            }
        }
    }

    return false;

found:
    if (!outStart->m_hour && !outStart->m_minute) {
        outStart->AddMinutes(1);
    }

    if (!outEnd->m_hour && !outEnd->m_minute) {
        outEnd->AddMinutes(-1);
    }

    return true;
}

// VERIFY: 649c97da manual
// OFFSET: 0x005BFD20
static int QSortEvents(const void* a, const void* b) {
    const CalendarDayEvent* left = *static_cast<CalendarDayEvent* const*>(a);
    const CalendarDayEvent* right = *static_cast<CalendarDayEvent* const*>(b);

    if ((left->m_flags & 0x1) && !(right->m_flags & 0x1)) {
        return -1;
    }

    if (!(left->m_flags & 0x1) && (right->m_flags & 0x1)) {
        return 1;
    }

    if ((left->m_type & 0x543) && !(right->m_type & 0x543)) {
        return -1;
    }

    if (!(left->m_type & 0x543) && (right->m_type & 0x543)) {
        return 1;
    }

    uint32_t leftHoliday = left->m_type & 0x8;

    if (leftHoliday && !(right->m_type & 0x8)) {
        return -1;
    }

    if (!leftHoliday && (right->m_type & 0x8)) {
        return 1;
    }

    int32_t rightPriority = -1;
    int32_t leftPriority = -1;
    HolidaysRec* leftRec = nullptr;

    if (leftHoliday) {
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(static_cast<uint32_t>(left->m_eventID), HASHKEY_NONE());

        if (dynamic) {
            leftPriority = dynamic->m_priority;
        } else {
            leftRec = g_holidaysDB.GetRecord(static_cast<int32_t>(left->m_eventID));

            if (leftRec) {
                leftPriority = leftRec->m_priority;
            }
        }
    }

    HolidaysRec* rightRec = nullptr;

    if (right->m_type & 0x8) {
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(static_cast<uint32_t>(right->m_eventID), HASHKEY_NONE());

        if (dynamic) {
            rightPriority = dynamic->m_priority;
        } else {
            rightRec = g_holidaysDB.GetRecord(static_cast<int32_t>(right->m_eventID));

            if (rightRec) {
                rightPriority = rightRec->m_priority;
            }
        }
    }

    int32_t result = rightPriority - leftPriority;

    if (result) {
        return result;
    }

    result = left->GetSequenceType() - right->GetSequenceType();

    if (result) {
        return result;
    }

    result = left->m_startTime - right->m_startTime;

    if (result) {
        return result;
    }

    const char* leftName = left->m_title;
    const char* rightName = right->m_title;

    if (leftRec) {
        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(leftRec->m_holidayNameID);

        if (name) {
            leftName = name->m_name;
        }
    } else if (left->m_type & 0x280) {
        MapRec* map = g_mapDB.GetRecord(left->m_holidayID);

        if (map) {
            leftName = map->m_mapName;
        }
    }

    if (rightRec) {
        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(rightRec->m_holidayNameID);

        if (name) {
            rightName = name->m_name;
        }
    } else if (right->m_type & 0x280) {
        MapRec* map = g_mapDB.GetRecord(right->m_holidayID);

        if (map) {
            rightName = map->m_mapName;
        }
    }

    return SStrCmpUTF8I(leftName, rightName, STORM_MAX_STR);
}

// VERIFY: e79c8713 manual
// OFFSET: 0x005BFF30
void CGCalendar::UpdateDisplayList(bool signal) {
    if (!CGCalendar::m_absMonthDays) {
        int32_t year = g_clientGameTime.m_year;
        int32_t month = g_clientGameTime.m_month;
        CGCalendar::m_absMonth = month;
        CGCalendar::m_absYear = year;

        if (year < 30) {
            if (year <= 4) {
                year = 4;
                CGCalendar::m_absYear = 4;

                if (g_clientGameTime.m_month <= 10) {
                    month = 10;
                    CGCalendar::m_absMonth = 10;
                }
            }
        } else {
            year = 30;
            CGCalendar::m_absYear = 30;
        }

        int32_t weekDay;
        CGCalendar::GetDaysInMonth(month, year, &CGCalendar::m_absMonthDays, &weekDay);
        CGCalendar::CalculateEvents(true);
        return;
    }

    int32_t month = (CGCalendar::m_absMonth + 11) % 12;
    int32_t year = month == 11 ? CGCalendar::m_absYear - 1 : CGCalendar::m_absYear;

    for (uint32_t m = 0; m < 3; m++) {
        for (uint32_t d = 0; d < 31; d++) {
            TSGrowableArray<CalendarDayEvent*>& list = s_days[m][d];
            uint32_t count = 0;
            list.m_count = 0;

            WowTime date;
            date.m_month = month;
            date.m_monthDay = d;
            date.m_year = year;
            uint32_t hash;
            WowTime::WowEncodeTime(hash, &date);

            for (uint32_t t = 0; t < 8; t++) {
                CVar** filter = s_eventFilters[t];

                if (filter && !(*filter && (*filter)->m_intValue)) {
                    continue;
                }

                CALENDAREVENTHASH* entry = s_eventTables.m_tables[t].Ptr(hash, HASHKEY_NONE());

                if (!entry) {
                    continue;
                }

                uint32_t numEvents = entry->m_events.Count();
                list.SetCount(count + numEvents);

                for (uint32_t k = 0; k < numEvents; k++) {
                    CalendarDayEvent* event = entry->m_events.m_data[k];

                    if (!FriendList::s_friendList->IsIgnored(event->m_creatorGUID, false)) {
                        list.m_data[count++] = event;
                    }
                }
            }

            list.SetCount(count);
        }

        for (uint32_t d = 0; d < 31; d++) {
            qsort(s_days[m][d].m_data, s_days[m][d].Count(), sizeof(CalendarDayEvent*), QSortEvents);
        }

        month = (month + 1) % 12;

        if (!month && ++year >= 31) {
            break;
        }
    }

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, nullptr);
    }
}

// VERIFY: c7d9d86f manual
// OFFSET: 0x005C0240
void CGCalendar::SetFilter(CVar* var, int32_t value) {
    if (!var || !CGCalendar::m_absMonthDays) {
        return;
    }

    int32_t month = (CGCalendar::m_absMonth + 11) % 12;
    int32_t year = month == 11 ? CGCalendar::m_absYear - 1 : CGCalendar::m_absYear;

    for (uint32_t m = 0; m < 3; m++) {
        for (uint32_t d = 0; d < 31; d++) {
            TSGrowableArray<CalendarDayEvent*>& list = s_days[m][d];
            uint32_t count = 0;
            list.m_count = 0;

            WowTime date;
            date.m_month = month;
            date.m_monthDay = d;
            date.m_year = year;
            uint32_t hash;
            WowTime::WowEncodeTime(hash, &date);

            for (uint32_t t = 0; t < 8; t++) {
                CVar** filter = s_eventFilters[t];

                if (filter && *filter && !(*filter == var ? value : (*filter)->m_intValue)) {
                    continue;
                }

                CALENDAREVENTHASH* entry = s_eventTables.m_tables[t].Ptr(hash, HASHKEY_NONE());

                if (!entry) {
                    continue;
                }

                uint32_t numEvents = entry->m_events.Count();
                list.SetCount(count + numEvents);

                for (uint32_t k = 0; k < numEvents; k++) {
                    CalendarDayEvent* event = entry->m_events.m_data[k];

                    if (!FriendList::s_friendList->IsIgnored(event->m_creatorGUID, false)) {
                        list.m_data[count++] = event;
                    }
                }
            }

            list.SetCount(count);
        }

        for (uint32_t d = 0; d < 31; d++) {
            qsort(s_days[m][d].m_data, s_days[m][d].Count(), sizeof(CalendarDayEvent*), QSortEvents);
        }

        month = (month + 1) % 12;

        if (!month && ++year >= 31) {
            break;
        }
    }
}

// VERIFY: 6146a69c manual
// OFFSET: 0x005C04F0
void CGCalendar::InitHolidays() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    int32_t factionMask = 0;
    int32_t faction = player->GetPVPFactionIndex();

    if (faction == 1) {
        factionMask = 1;
    } else if (faction == 0) {
        factionMask = 2;
    }

    if (!(factionMask & 3)) {
        return;
    }

    int8_t region = static_cast<int8_t>(s_region);

    for (int32_t i = 0; i < g_holidaysDB.m_numRecords; i++) {
        HolidaysRec* holiday = i < 0 || i >= g_holidaysDB.m_numRecords ? nullptr : &g_holidaysDB.m_records[i];
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(holiday->m_id, HASHKEY_NONE());
        int32_t regionMask = dynamic ? dynamic->m_region : holiday->m_region;

        if (regionMask && !((1 << (region - 1)) & regionMask)) {
            continue;
        }

        int32_t looping;
        int32_t filterType;
        int32_t* flags;
        int32_t* durations;

        if (dynamic) {
            looping = dynamic->m_looping;
            flags = dynamic->m_calendarFlags;
            durations = dynamic->m_duration;
            filterType = dynamic->m_calendarFilterType;
        } else {
            looping = holiday->m_looping;
            filterType = holiday->m_calendarFilterType;
            flags = holiday->m_calendarFlags;
            durations = holiday->m_duration;
        }

        for (int32_t j = 0; j < 10; j++) {
            if (!(factionMask & flags[j])) {
                continue;
            }

            CALENDARHOLIDAYNODE* node = s_holidays.NewNode(2, 0, 0);

            if (!node) {
                continue;
            }

            node->m_holiday = holiday;
            node->m_index = j;

            if (looping) {
                int32_t total = durations[j];

                if (!total) {
                    total = 24;
                }

                node->m_duration = total;

                for (int32_t k = 0; k < 10; k++) {
                    if (k != j) {
                        node->m_duration += durations[k];
                    }

                    if (!k && !durations[0]) {
                        node->m_duration += 3;
                    }
                }

                node->m_duration *= 60;
            }

            if (filterType == 0) {
                node->m_type = 5;
            } else if (filterType == 1) {
                node->m_type = 6;
            } else if (filterType == 2) {
                node->m_type = 7;
            }
        }
    }
}

// VERIFY: 0650d640 manual
// OFFSET: 0x005C2030
void CGCalendar::InsertEvent(CalendarDayEvent* event, int32_t table) {
    uint32_t originalEnd = event->m_endTime;
    WowTime start;
    WowTime end;
    WowTime::WowDecodeTime(event->m_startTime, &start);

    if (event->m_endTime) {
        WowTime::WowDecodeTime(event->m_endTime, &end);

        if (end.m_year == -1) {
            end.m_year = 31;
        }
    } else {
        end = start;
    }

    WowTime day = start;
    uint32_t numDays = 0;

    while (day <= end) {
        numDays++;
        day.AddDays(1, false);
    }

    int32_t month = start.m_month;
    uint32_t sequence = 0;
    bool first = true;
    TSHashTable<CALENDAREVENTHASH, HASHKEY_NONE>* events = &s_eventTables.m_tables[table];

    do {
        WowTime key;
        key.m_month = month;
        key.m_monthDay = start.m_monthDay;
        key.m_year = start.m_year;
        uint32_t hash;
        WowTime::WowEncodeTime(hash, &key);
        CALENDAREVENTHASH* entry = events->Ptr(hash, HASHKEY_NONE());

        if (!entry) {
            s_eventTables.m_counts[table]++;
            entry = events->New(hash, HASHKEY_NONE(), 0, 0);
        }

        if (!first) {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);

            if (mem) {
                memcpy(mem, event, sizeof(CalendarDayEvent));
                event = static_cast<CalendarDayEvent*>(mem);
            } else {
                event = nullptr;
            }
        }

        event->m_sequenceIndex = sequence;
        event->m_sequenceLength = numDays;
        sequence++;

        CalendarDayEvent** slot = entry->m_events.New();

        if (slot) {
            *slot = event;
        }

        if (first) {
            memset(&start, 0xFF, 12);
            first = false;
        }

        start.AddDays(1, false);

        if (start > end) {
            break;
        }

        month = start.m_month;
    } while (start.m_month != end.m_month || start.m_monthDay != end.m_monthDay || end.m_hour || end.m_minute);

    if (numDays > 1) {
        event->m_startTime = originalEnd;
    }
}

// VERIFY: 1cd1e699 manual
// OFFSET: 0x005C23C0
void CGCalendar::CalculateEvents(bool signal) {
    s_eventTables.m_counts[4] = 0;
    s_eventTables.m_tables[4].InternalClear(0);
    s_eventTables.m_counts[5] = 0;
    s_eventTables.m_tables[5].InternalClear(0);
    s_eventTables.m_counts[6] = 0;
    s_eventTables.m_tables[6].InternalClear(0);
    s_eventTables.m_counts[7] = 0;
    s_eventTables.m_tables[7].InternalClear(0);
    s_eventTables.m_counts[3] = 0;
    s_eventTables.m_tables[3].InternalClear(0);

    WowTime start;
    WowTime end;
    WowTime rangeStart;
    WowTime rangeEnd;
    rangeStart.m_year = CGCalendar::m_absYear;
    rangeStart.m_month = CGCalendar::m_absMonth;
    rangeStart.m_monthDay = 0;
    rangeStart.SetHourAndMinutes(0u, 0u);
    rangeStart.AddDays(-6, false);
    rangeEnd.m_month = CGCalendar::m_absMonth;
    rangeEnd.m_monthDay = 0;
    rangeEnd.m_year = CGCalendar::m_absYear;
    rangeEnd.SetHourAndMinutes(0u, 0u);
    rangeEnd.AddDays(42, false);

    if (!s_holidays.Head()) {
        CGCalendar::InitHolidays();
    }

    for (CALENDARHOLIDAYNODE* node = s_holidays.Head(); node; node = s_holidays.Next(node)) {
        bool weekly = false;

        if (ComputeEventOccurrence(&g_clientGameTime, &rangeStart, &rangeEnd, &end, node->m_holiday, node->m_index, &weekly, node->m_duration, &start)) {
            if (weekly) {
                node->m_duration = 10080;
            } else {
                start.AddDays(0, false);
                end.AddDays(0, false);
            }

            WowTime::WowEncodeTime(node->m_startTime, &start);
            WowTime::WowEncodeTime(node->m_endTime, &end);
        } else {
            node->m_startTime = 0;
            node->m_endTime = 0;
        }
    }

    for (CALENDARHOLIDAYNODE* node = s_holidays.Head(); node; node = s_holidays.Next(node)) {
        if (!node->m_startTime) {
            continue;
        }

        HolidaysRec* holiday = g_holidaysDB.GetRecord(node->m_holiday->m_id);

        if (!holiday) {
            continue;
        }

        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(holiday->m_holidayNameID);

        if (!name || !name->m_name) {
            continue;
        }

        if (node->m_duration) {
            WowTime::WowDecodeTime(node->m_startTime, &start);
            WowTime::WowDecodeTime(node->m_endTime, &end);

            if (end.m_year == -1) {
                end.m_year = 31;
            }

            if (!(start < rangeEnd)) {
                continue;
            }

            while (start.m_year < 31) {
                uint32_t startTime;
                uint32_t endTime;
                WowTime::WowEncodeTime(startTime, &start);
                WowTime::WowEncodeTime(endTime, &end);

                void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
                CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : nullptr;
                event->m_eventID = node->m_holiday->m_id;
                event->m_title[0] = '\0';
                event->m_startTime = startTime;
                event->m_endTime = endTime;
                event->m_type = 8;
                CGCalendar::InsertEvent(event, node->m_type);
                start.AddHolidayDuration(node->m_duration);
                end.AddHolidayDuration(node->m_duration);

                if (!(start < rangeEnd)) {
                    break;
                }
            }
        } else {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
            CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : nullptr;
            event->m_eventID = node->m_holiday->m_id;
            event->m_title[0] = '\0';
            event->m_startTime = node->m_startTime;
            event->m_endTime = node->m_endTime;
            event->m_type = 8;
            CGCalendar::InsertEvent(event, node->m_type);
        }
    }

    WowTime reset;

    for (CALENDARRAIDRESETNODE* node = s_raidResets.Head(); node; node = s_raidResets.Next(node)) {
        if (!node->m_startTime || !node->m_interval) {
            continue;
        }

        WowTime::WowDecodeTime(node->m_startTime, &reset);
        reset.AddHolidayDuration(node->m_interval);

        while (reset < rangeEnd && reset.m_year < 31) {
            if (reset > rangeStart) {
                void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
                CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : nullptr;
                event->m_holidayID = node->m_mapID;
                event->m_title[0] = '\0';
                event->m_type = 512;
                WowTime::WowEncodeTime(event->m_startTime, &reset);
                CGCalendar::InsertEvent(event, 3);
            }

            reset.AddHolidayDuration(node->m_interval);
        }
    }

    CGCalendar::UpdateDisplayList(signal);
}

// VERIFY: 0e8aff06 manual
// OFFSET: 0x005B7B30
bool CGCalendar::ValidateDate(const WowTime* date) {
    WowTime check = *date;
    check.m_tzHours = date->m_tzHours;

    WowTime limit;
    limit.m_month = 10;
    limit.m_monthDay = 23;
    limit.m_year = 4;
    limit.AddDays(0, false);
    limit.SetHourAndMinutes(0u, 0u);
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);

    if (check < limit) {
        return false;
    }

    limit.m_month = 0;
    limit.m_monthDay = 0;
    limit.m_year = 31;
    limit.AddDays(-1, false);
    limit.SetHourAndMinutes(23u, 59u);
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);
    return !(check > limit);
}

// VERIFY: 06530d5a auto
// OFFSET: 0x005B91A0
bool CGCalendar::GetDayEvent(uint32_t offset, uint32_t day) {
    if (offset + 1 > 2 || day >= 31) {
        return false;
    }

    WowTime date;
    int32_t month = 0;
    int32_t year = 0;
    int32_t days;
    int32_t weekDay;
    CGCalendar::GetMonth(offset, &month, &year, &days, &weekDay);
    date.SetDate(month, day, year);
    return CGCalendar::ValidateDate(&date);
}

// VERIFY: b060f4e8 auto
// OFFSET: 0x005B9220
int32_t CGCalendar::GetNumDayEvents(uint32_t offset, uint32_t day) {
    if (!CGCalendar::GetDayEvent(offset, day)) {
        return 0;
    }

    return s_days[offset + 1][day].Count();
}

// VERIFY: 28aef737 auto
// OFFSET: 0x005B92B0
int32_t CGCalendar::GetFirstPendingInviteIndex(uint32_t offset, uint32_t day) {
    if (!CGCalendar::GetDayEvent(offset, day)) {
        return -1;
    }

    TSGrowableArray<CalendarDayEvent*>& list = s_days[offset + 1][day];

    for (uint32_t i = 0; i < list.Count(); i++) {
        if (list.m_data[i] && (list.m_data[i]->m_flags & 0x1)) {
            return i;
        }
    }

    return -1;
}
