#include "Calendar.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Game/GameTime.h"

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
