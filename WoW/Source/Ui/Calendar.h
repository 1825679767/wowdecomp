#ifndef WOW_SOURCE_UI_CALENDAR_H
#define WOW_SOURCE_UI_CALENDAR_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

class CDataStore;
class CGCalendarEvent;
class CVar;
struct lua_State;

int32_t PH_CALENDAR_GROUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGCalendar {
    public:
    static void EnterWorld();
    static uint8_t m_actionPending;
    static CGCalendarEvent* m_curEvent;
    static uint8_t m_initialize;
    static uint8_t m_openedCalendar;
    static uint8_t unk_C207F2;
    static int32_t m_numInitialPending;
    static void InitializeGame();
    static void SetFilter(CVar* var, int32_t value);
};

int32_t Script_CalendarGetMinDate(lua_State* L);
int32_t Script_CalendarGetMaxDate(lua_State* L);
int32_t Script_CalendarGetMinHistoryDate(lua_State* L);
int32_t Script_CalendarGetMaxCreateDate(lua_State* L);
int32_t Script_CalendarGetMonth(lua_State* L);
int32_t Script_CalendarGetAbsMonth(lua_State* L);
int32_t Script_CalendarSetMonth(lua_State* L);
int32_t Script_CalendarSetAbsMonth(lua_State* L);
int32_t Script_CalendarGetNumDayEvents(lua_State* L);
int32_t Script_CalendarGetDayEvent(lua_State* L);
int32_t Script_CalendarGetDayEventSequenceInfo(lua_State* L);
int32_t Script_CalendarGetFirstPendingInvite(lua_State* L);
int32_t Script_CalendarOpenEvent(lua_State* L);
int32_t Script_CalendarGetEventIndex(lua_State* L);
int32_t Script_CalendarCloseEvent(lua_State* L);
int32_t Script_CalendarGetEventInfo(lua_State* L);
int32_t Script_CalendarGetHolidayInfo(lua_State* L);
int32_t Script_CalendarGetRaidInfo(lua_State* L);
int32_t Script_CalendarGetNumPendingInvites(lua_State* L);
int32_t Script_CalendarEventGetNumInvites(lua_State* L);
int32_t Script_CalendarEventGetInvite(lua_State* L);
int32_t Script_CalendarEventGetInviteResponseTime(lua_State* L);
int32_t Script_CalendarAddEvent(lua_State* L);
int32_t Script_CalendarNewEvent(lua_State* L);
int32_t Script_CalendarMassInviteGuild(lua_State* L);
int32_t Script_CalendarMassInviteArenaTeam(lua_State* L);
int32_t Script_CalendarNewGuildAnnouncement(lua_State* L);
int32_t Script_CalendarNewGuildEvent(lua_State* L);
int32_t Script_CalendarDefaultGuildFilter(lua_State* L);
int32_t Script_CalendarUpdateEvent(lua_State* L);
int32_t Script_CalendarRemoveEvent(lua_State* L);
int32_t Script_CalendarEventSelectInvite(lua_State* L);
int32_t Script_CalendarEventGetSelectedInvite(lua_State* L);
int32_t Script_CalendarContextSelectEvent(lua_State* L);
int32_t Script_CalendarContextDeselectEvent(lua_State* L);
int32_t Script_CalendarContextGetEventIndex(lua_State* L);
int32_t Script_CalendarContextInviteIsPending(lua_State* L);
int32_t Script_CalendarContextInviteModeratorStatus(lua_State* L);
int32_t Script_CalendarContextInviteStatus(lua_State* L);
int32_t Script_CalendarContextInviteType(lua_State* L);
int32_t Script_CalendarContextInviteAvailable(lua_State* L);
int32_t Script_CalendarContextInviteTentative(lua_State* L);
int32_t Script_CalendarContextInviteDecline(lua_State* L);
int32_t Script_CalendarContextInviteRemove(lua_State* L);
int32_t Script_CalendarContextEventSignUp(lua_State* L);
int32_t Script_CalendarContextEventRemove(lua_State* L);
int32_t Script_CalendarContextEventCopy(lua_State* L);
int32_t Script_CalendarContextEventPaste(lua_State* L);
int32_t Script_CalendarContextEventClipboard(lua_State* L);
int32_t Script_CalendarContextEventCanComplain(lua_State* L);
int32_t Script_CalendarContextEventComplain(lua_State* L);
int32_t Script_CalendarContextEventCanEdit(lua_State* L);
int32_t Script_CalendarContextEventGetCalendarType(lua_State* L);
int32_t Script_CalendarEventInvite(lua_State* L);
int32_t Script_CalendarEventRemoveInvite(lua_State* L);
int32_t Script_CalendarEventAvailable(lua_State* L);
int32_t Script_CalendarEventTentative(lua_State* L);
int32_t Script_CalendarEventDecline(lua_State* L);
int32_t Script_CalendarEventSignUp(lua_State* L);
int32_t Script_CalendarEventSortInvites(lua_State* L);
int32_t Script_CalendarEventGetInviteSortCriterion(lua_State* L);
int32_t Script_CalendarEventGetStatusOptions(lua_State* L);
int32_t Script_CalendarEventSetStatus(lua_State* L);
int32_t Script_CalendarEventSetModerator(lua_State* L);
int32_t Script_CalendarEventClearModerator(lua_State* L);
int32_t Script_CalendarEventCanModerate(lua_State* L);
int32_t Script_CalendarEventIsModerator(lua_State* L);
int32_t Script_CalendarEventGetTypes(lua_State* L);
int32_t Script_CalendarEventGetRepeatOptions(lua_State* L);
int32_t Script_CalendarEventSetTitle(lua_State* L);
int32_t Script_CalendarEventSetDescription(lua_State* L);
int32_t Script_CalendarEventSetType(lua_State* L);
int32_t Script_CalendarEventSetRepeatOption(lua_State* L);
int32_t Script_CalendarEventSetSize(lua_State* L);
int32_t Script_CalendarEventSetDate(lua_State* L);
int32_t Script_CalendarEventSetTime(lua_State* L);
int32_t Script_CalendarEventSetLockoutDate(lua_State* L);
int32_t Script_CalendarEventSetLockoutTime(lua_State* L);
int32_t Script_CalendarEventSetTextureID(lua_State* L);
int32_t Script_CalendarEventSetLocked(lua_State* L);
int32_t Script_CalendarEventClearLocked(lua_State* L);
int32_t Script_CalendarEventSetAutoApprove(lua_State* L);
int32_t Script_CalendarEventClearAutoApprove(lua_State* L);
int32_t Script_CalendarEventGetTextures(lua_State* L);
int32_t Script_CalendarEventHasPendingInvite(lua_State* L);
int32_t Script_CalendarEventHaveSettingsChanged(lua_State* L);
int32_t Script_CalendarEventCanEdit(lua_State* L);
int32_t Script_CalendarEventGetCalendarType(lua_State* L);
int32_t Script_CalendarCanSendInvite(lua_State* L);
int32_t Script_CalendarCanAddEvent(lua_State* L);
int32_t Script_OpenCalendar(lua_State* L);
void CalendarRegisterScriptFunctions();

#endif
