#include "Calendar.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Game/GameTime.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00ACFFB4
static const char* s_monthNames[12] = { "MONTH_JANUARY", "MONTH_FEBRUARY", "MONTH_MARCH", "MONTH_APRIL", "MONTH_MAY", "MONTH_JUNE", "MONTH_JULY", "MONTH_AUGUST", "MONTH_SEPTEMBER", "MONTH_OCTOBER", "MONTH_NOVEMBER", "MONTH_DECEMBER" };

// STATIC: 0x00AD0014
const char* s_weekdayNames[7] = { "WEEKDAY_SUNDAY", "WEEKDAY_MONDAY", "WEEKDAY_TUESDAY", "WEEKDAY_WEDNESDAY", "WEEKDAY_THURSDAY", "WEEKDAY_FRIDAY", "WEEKDAY_SATURDAY" };

// VERIFY: 98948d66 auto
// OFFSET: 0x005B8C10
static int32_t Script_CalendarIsActionPending(lua_State* L) {
    lua_pushboolean(L, CGCalendar::m_actionPending);
    return 1;
}

// VERIFY: 9224a39b auto
// OFFSET: 0x005B80E0
static int32_t Script_CalendarGetMonthNames(lua_State* L) {
    lua_checkstack(L, 12);

    for (uint32_t i = 0; i < 12; i++) {
        lua_pushstring(L, FrameScript_GetText(s_monthNames[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 12;
}

// VERIFY: 735dbaf4 auto
// OFFSET: 0x005B8120
static int32_t Script_CalendarGetWeekdayNames(lua_State* L) {
    lua_checkstack(L, 7);

    for (uint32_t i = 0; i < 7; i++) {
        lua_pushstring(L, FrameScript_GetText(s_weekdayNames[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 7;
}

// VERIFY: 5afc3ec9 auto
// OFFSET: 0x005B8160
static int32_t Script_CalendarGetDate(lua_State* L) {
    lua_pushnumber(L, g_clientGameTime.m_weekDay + 1);
    lua_pushnumber(L, g_clientGameTime.m_month + 1);
    lua_pushnumber(L, g_clientGameTime.m_monthDay + 1);
    lua_pushnumber(L, g_clientGameTime.m_year + 2000);
    return 4;
}

// VERIFY: 095feebb manual
// OFFSET: 0x005BF830
static int32_t Script_CalendarGetNumPendingInvites(lua_State* L) {
    lua_pushnumber(L, CGCalendar::CountPendingInvites());
    return 1;
}

// STATIC: 0x00A18684
static const int32_t s_expansionMaxLevels[3] = { 60, 70, 80 };

// VERIFY: 93df37f7 auto
// OFFSET: 0x005B81F0
static int32_t Script_CalendarGetMinDate(lua_State* L) {
    WowTime date;
    date.m_month = 10;
    date.m_monthDay = 23;
    date.m_year = 4;
    date.AddDays(0, false);
    date.SetHourAndMinutes(0u, 0u);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// VERIFY: 7d9608cb auto
// OFFSET: 0x005B84A0
static int32_t Script_CalendarCloseEvent(lua_State* L) {
    CGCalendar::m_selectedEvent.m_eventID = 0;
    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.unk_0014 = 0;
    return 0;
}

// VERIFY: e71b5505 auto
// OFFSET: 0x005B8970
static int32_t Script_CalendarEventGetTypes(lua_State* L) {
    lua_checkstack(L, 5);

    for (uint32_t i = 0; i < 5; i++) {
        lua_pushstring(L, FrameScript_GetText(GetEventTypeString(i), -1, GENDER_NOT_APPLICABLE));
    }

    return 5;
}

// VERIFY: a996e5a9 auto
// OFFSET: 0x005B89C0
static int32_t Script_CalendarEventGetRepeatOptions(lua_State* L) {
    lua_checkstack(L, 4);

    for (uint32_t i = 0; i < 4; i++) {
        lua_pushstring(L, FrameScript_GetText(GetEventRepeatString(i), -1, GENDER_NOT_APPLICABLE));
    }

    return 4;
}

// VERIFY: 772e0e5f manual
// OFFSET: 0x005B99F0
static int32_t Script_CalendarGetMonth(lua_State* L) {
    int32_t offset = 0;

    if (lua_isnumber(L, 1)) {
        offset = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    int32_t month = 0;
    int32_t year = 0;
    int32_t days = 0;
    int32_t weekDay = 0;
    CGCalendar::GetMonth(offset, &month, &year, &days, &weekDay);
    lua_pushnumber(L, ++month);
    year += 2000;
    lua_pushnumber(L, year);
    lua_pushnumber(L, static_cast<uint32_t>(days));
    lua_pushnumber(L, weekDay + 1);
    return 4;
}

// VERIFY: 52664e31 auto
// OFFSET: 0x005B9F80
static int32_t Script_CalendarGetEventIndex(lua_State* L) {
    int32_t monthOffset;
    int32_t day;
    int32_t index;

    if (CGCalendar::ComputeEventIndex(&CGCalendar::m_selectedEvent, &monthOffset, &day, &index)) {
        lua_pushinteger(L, monthOffset);
        lua_pushinteger(L, day + 1);
        lua_pushinteger(L, index + 1);
    } else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }

    return 3;
}

// VERIFY: c93d6dde auto
// OFFSET: 0x005BA680
static int32_t Script_CalendarContextGetEventIndex(lua_State* L) {
    int32_t monthOffset;
    int32_t day;
    int32_t index;

    if (CGCalendar::ComputeEventIndex(&CGCalendar::m_contextEvent, &monthOffset, &day, &index)) {
        lua_pushinteger(L, monthOffset);
        lua_pushinteger(L, day + 1);
        lua_pushinteger(L, index + 1);
    } else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }

    return 3;
}

// VERIFY: 2581947b manual
// OFFSET: 0x005BCDA0
static int32_t Script_CalendarDefaultGuildFilter(lua_State* L) {
    if (ClntObjMgrGetActivePlayerObj()) {
        double level = s_expansionMaxLevels[ClientServices::Connection()->m_accountExpansion];
        lua_pushnumber(L, level);
        lua_pushnumber(L, level);
        lua_pushnumber(L, CGGuildInfo::m_numRanks);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// VERIFY: 078c9cf4 manual
// OFFSET: 0x005C43B0
static int32_t Script_CalendarSetAbsMonth(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarSetAbsMonth(month[, year])");
    }

    int32_t month = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t year = CGCalendar::m_absYear;

    if (CGCalendar::m_absYear < 0) {
        year = g_clientGameTime.m_year;
        CGCalendar::m_absYear = year;

        if (year < 0) {
            year = 4;
            CGCalendar::m_absYear = 4;
        }
    }

    if (lua_isnumber(L, 2)) {
        year = static_cast<int32_t>(lua_tonumber(L, 2)) - 2000;
    }

    int32_t weekDay;
    CGCalendar::ClampAbsMonthDay(month, year, &CGCalendar::m_absMonth, &CGCalendar::m_absYear, &CGCalendar::m_absMonthDays, &weekDay);
    CGCalendar::CalculateEvents(true);
    return 0;
}

// VERIFY: 90f38e2c auto
// OFFSET: 0x005B8410
static int32_t Script_CalendarGetMaxCreateDate(lua_State* L) {
    WowTime date;
    GameCalendarGetMaxCreateDate(&date);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// VERIFY: 6bc9ca17 auto
// OFFSET: 0x005B8B30
static int32_t Script_CalendarCanSendInvite(lua_State* L) {
    uint32_t now;

    if (CGCalendar::m_actionPending || ((!unk_BD09AC || !unk_BD09AC->m_intValue) && (now = OsGetAsyncTimeMs(), CGCalendar::m_lastInviteTime) && now - CGCalendar::m_lastInviteTime < 2000)) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    }

    return 1;
}

// VERIFY: 3aba7d21 auto
// OFFSET: 0x005BCC00
static int32_t Script_CalendarSetMonth(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarSetMonth(offsetMonths)");
    }

    int32_t weekDay;
    CGCalendar::GetMonth(static_cast<int32_t>(lua_tonumber(L, 1)), &CGCalendar::m_absMonth, &CGCalendar::m_absYear, &CGCalendar::m_absMonthDays, &weekDay);
    CGCalendar::CalculateEvents(true);
    return 0;
}

// VERIFY: 6784fd0d manual
// OFFSET: 0x005B9BF0
static int32_t Script_CalendarGetNumDayEvents(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarGetNumDayEvents([-1,0,1], monthDay)");
    }

    uint32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    lua_pushnumber(L, CGCalendar::GetNumDayEvents(offset, day));
    return 1;
}

// VERIFY: 7681b03c manual
// OFFSET: 0x005B9E10
static int32_t Script_CalendarGetFirstPendingInvite(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarGetDay([-1,0,1], monthDay)");
    }

    uint32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    lua_pushinteger(L, CGCalendar::GetFirstPendingInviteIndex(offset, day) + 1);
    return 1;
}

// STATIC: 0x00AD0030
static FrameScript_Method s_ScriptFunctions[] = {
    { "CalendarGetMonthNames", &Script_CalendarGetMonthNames },
    { "CalendarGetWeekdayNames", &Script_CalendarGetWeekdayNames },
    { "CalendarGetDate", &Script_CalendarGetDate },
    { "CalendarGetMinDate", &Script_CalendarGetMinDate },
    { "CalendarGetMaxDate", &Script_CalendarGetMaxDate },
    { "CalendarGetMinHistoryDate", &Script_CalendarGetMinHistoryDate },
    { "CalendarGetMaxCreateDate", &Script_CalendarGetMaxCreateDate },
    { "CalendarGetMonth", &Script_CalendarGetMonth },
    { "CalendarGetAbsMonth", &Script_CalendarGetAbsMonth },
    { "CalendarSetMonth", &Script_CalendarSetMonth },
    { "CalendarSetAbsMonth", &Script_CalendarSetAbsMonth },
    { "CalendarGetNumDayEvents", &Script_CalendarGetNumDayEvents },
    { "CalendarGetDayEvent", &Script_CalendarGetDayEvent },
    { "CalendarGetDayEventSequenceInfo", &Script_CalendarGetDayEventSequenceInfo },
    { "CalendarGetFirstPendingInvite", &Script_CalendarGetFirstPendingInvite },
    { "CalendarOpenEvent", &Script_CalendarOpenEvent },
    { "CalendarGetEventIndex", &Script_CalendarGetEventIndex },
    { "CalendarCloseEvent", &Script_CalendarCloseEvent },
    { "CalendarGetEventInfo", &Script_CalendarGetEventInfo },
    { "CalendarGetHolidayInfo", &Script_CalendarGetHolidayInfo },
    { "CalendarGetRaidInfo", &Script_CalendarGetRaidInfo },
    { "CalendarGetNumPendingInvites", &Script_CalendarGetNumPendingInvites },
    { "CalendarEventGetNumInvites", &Script_CalendarEventGetNumInvites },
    { "CalendarEventGetInvite", &Script_CalendarEventGetInvite },
    { "CalendarEventGetInviteResponseTime", &Script_CalendarEventGetInviteResponseTime },
    { "CalendarAddEvent", &Script_CalendarAddEvent },
    { "CalendarNewEvent", &Script_CalendarNewEvent },
    { "CalendarMassInviteGuild", &Script_CalendarMassInviteGuild },
    { "CalendarMassInviteArenaTeam", &Script_CalendarMassInviteArenaTeam },
    { "CalendarNewGuildAnnouncement", &Script_CalendarNewGuildAnnouncement },
    { "CalendarNewGuildEvent", &Script_CalendarNewGuildEvent },
    { "CalendarDefaultGuildFilter", &Script_CalendarDefaultGuildFilter },
    { "CalendarUpdateEvent", &Script_CalendarUpdateEvent },
    { "CalendarRemoveEvent", &Script_CalendarRemoveEvent },
    { "CalendarEventSelectInvite", &Script_CalendarEventSelectInvite },
    { "CalendarEventGetSelectedInvite", &Script_CalendarEventGetSelectedInvite },
    { "CalendarContextSelectEvent", &Script_CalendarContextSelectEvent },
    { "CalendarContextDeselectEvent", &Script_CalendarContextDeselectEvent },
    { "CalendarContextGetEventIndex", &Script_CalendarContextGetEventIndex },
    { "CalendarContextInviteIsPending", &Script_CalendarContextInviteIsPending },
    { "CalendarContextInviteModeratorStatus", &Script_CalendarContextInviteModeratorStatus },
    { "CalendarContextInviteStatus", &Script_CalendarContextInviteStatus },
    { "CalendarContextInviteType", &Script_CalendarContextInviteType },
    { "CalendarContextInviteAvailable", &Script_CalendarContextInviteAvailable },
    { "CalendarContextInviteTentative", &Script_CalendarContextInviteTentative },
    { "CalendarContextInviteDecline", &Script_CalendarContextInviteDecline },
    { "CalendarContextInviteRemove", &Script_CalendarContextInviteRemove },
    { "CalendarContextEventSignUp", &Script_CalendarContextEventSignUp },
    { "CalendarContextEventRemove", &Script_CalendarContextEventRemove },
    { "CalendarContextEventCopy", &Script_CalendarContextEventCopy },
    { "CalendarContextEventPaste", &Script_CalendarContextEventPaste },
    { "CalendarContextEventClipboard", &Script_CalendarContextEventClipboard },
    { "CalendarContextEventCanComplain", &Script_CalendarContextEventCanComplain },
    { "CalendarContextEventComplain", &Script_CalendarContextEventComplain },
    { "CalendarContextEventCanEdit", &Script_CalendarContextEventCanEdit },
    { "CalendarContextEventGetCalendarType", &Script_CalendarContextEventGetCalendarType },
    { "CalendarEventInvite", &Script_CalendarEventInvite },
    { "CalendarEventRemoveInvite", &Script_CalendarEventRemoveInvite },
    { "CalendarEventAvailable", &Script_CalendarEventAvailable },
    { "CalendarEventTentative", &Script_CalendarEventTentative },
    { "CalendarEventDecline", &Script_CalendarEventDecline },
    { "CalendarEventSignUp", &Script_CalendarEventSignUp },
    { "CalendarEventSortInvites", &Script_CalendarEventSortInvites },
    { "CalendarEventGetInviteSortCriterion", &Script_CalendarEventGetInviteSortCriterion },
    { "CalendarEventGetStatusOptions", &Script_CalendarEventGetStatusOptions },
    { "CalendarEventSetStatus", &Script_CalendarEventSetStatus },
    { "CalendarEventSetModerator", &Script_CalendarEventSetModerator },
    { "CalendarEventClearModerator", &Script_CalendarEventClearModerator },
    { "CalendarEventCanModerate", &Script_CalendarEventCanModerate },
    { "CalendarEventIsModerator", &Script_CalendarEventIsModerator },
    { "CalendarEventGetTypes", &Script_CalendarEventGetTypes },
    { "CalendarEventGetRepeatOptions", &Script_CalendarEventGetRepeatOptions },
    { "CalendarEventSetTitle", &Script_CalendarEventSetTitle },
    { "CalendarEventSetDescription", &Script_CalendarEventSetDescription },
    { "CalendarEventSetType", &Script_CalendarEventSetType },
    { "CalendarEventSetRepeatOption", &Script_CalendarEventSetRepeatOption },
    { "CalendarEventSetSize", &Script_CalendarEventSetSize },
    { "CalendarEventSetDate", &Script_CalendarEventSetDate },
    { "CalendarEventSetTime", &Script_CalendarEventSetTime },
    { "CalendarEventSetLockoutDate", &Script_CalendarEventSetLockoutDate },
    { "CalendarEventSetLockoutTime", &Script_CalendarEventSetLockoutTime },
    { "CalendarEventSetTextureID", &Script_CalendarEventSetTextureID },
    { "CalendarEventSetLocked", &Script_CalendarEventSetLocked },
    { "CalendarEventClearLocked", &Script_CalendarEventClearLocked },
    { "CalendarEventSetAutoApprove", &Script_CalendarEventSetAutoApprove },
    { "CalendarEventClearAutoApprove", &Script_CalendarEventClearAutoApprove },
    { "CalendarEventGetTextures", &Script_CalendarEventGetTextures },
    { "CalendarEventHasPendingInvite", &Script_CalendarEventHasPendingInvite },
    { "CalendarEventHaveSettingsChanged", &Script_CalendarEventHaveSettingsChanged },
    { "CalendarEventCanEdit", &Script_CalendarEventCanEdit },
    { "CalendarEventGetCalendarType", &Script_CalendarEventGetCalendarType },
    { "CalendarCanSendInvite", &Script_CalendarCanSendInvite },
    { "CalendarCanAddEvent", &Script_CalendarCanAddEvent },
    { "CalendarIsActionPending", &Script_CalendarIsActionPending },
    { "OpenCalendar", &Script_OpenCalendar },
};

// VERIFY: 70bc33d7 auto
// OFFSET: 0x005B8C30
void CalendarRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: e4c5acbe auto
// OFFSET: 0x005B8C60
void CalendarUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
