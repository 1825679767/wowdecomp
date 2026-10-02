#ifndef WOW_SOURCE_UI_CALENDAR_H
#define WOW_SOURCE_UI_CALENDAR_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
class CGCalendarEvent;
struct CVar;
struct lua_State;

struct CalendarEventClipboard {
    /* 0000 */ uint64_t m_eventID;
    /* 0008 */ uint8_t unk_0008[200];
};

int32_t PH_CALENDAR_GROUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
void PH_SMSG_CALENDAR_SEND_CALENDAR(CDataStore* msg);

struct CALENDARHOLIDAYNODE : public TSLinkedNode<CALENDARHOLIDAYNODE> {
};

struct CALENDARRAIDRESETNODE : public TSLinkedNode<CALENDARRAIDRESETNODE> {
};

struct DYNAMICHOLIDAYHASH : public TSHashObject<DYNAMICHOLIDAYHASH, HASHKEY_NONE> {
};

struct CALENDAREVENTHASH : public TSHashObject<CALENDAREVENTHASH, HASHKEY_NONE> {
};

struct CalendarDayEvent {
    /* 0000 */ uint64_t m_eventID;
    /* 0008 */ uint8_t unk_0008[128];
    /* 0088 */ uint32_t m_startTime;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ uint32_t m_endTime;
    /* 0094 */ uint32_t m_holidayID;
    /* 0098 */ uint32_t m_type;
    /* 009C */ uint8_t unk_009C[52];
};

struct CalendarDayInfo {
    /* 0000 */ uint32_t m_numEvents;
    /* 0004 */ CalendarDayEvent** m_events;
    /* 0008 */ uint32_t unk_0008[2];
};

struct CalendarEventSelection {
    /* 0000 */ uint64_t m_eventID;
    /* 0008 */ uint32_t m_startTime;
    /* 000C */ uint32_t m_endTime;
    /* 0010 */ uint32_t m_holidayID;
    /* 0014 */ int32_t unk_0014;
};

class CalendarEventTables {
    public:
    /* 0000 */ TSHashTable<CALENDAREVENTHASH, HASHKEY_NONE> m_tables[8];
    /* 0140 */ int32_t m_counts[8];

    void Clear();
};

struct CalendarPendingInvite : public TSLinkedNode<CalendarPendingInvite> {
    /* 0008 */ uint8_t unk_0008[16];
    /* 0018 */ WOWGUID m_inviterGuid;
};

class CGCalendar {
    public:
    static void GuildChanged();
    static uint32_t CountPendingInvites();
    static void UpdateDisplayList(int32_t a1);
    static uint8_t m_actionPending;
    static void CalendarError(const char* error, uintptr_t arg);
    static size_t GetDateTimeText(char* buffer, uint32_t size, uint32_t time);
    static CGCalendarEvent* m_curEvent;
    static uint8_t m_initialize;
    static uint8_t m_openedCalendar;
    static uint8_t unk_C207F2;
    static int32_t m_numInitialPending;

    static void EnterWorld();
    static void InitializeGame();
    static void ShutdownGame();
    static void QueryPendingInvites();
    static void SetFilter(CVar* var, int32_t value);
    static void ProcessGetEvent(CDataStore* msg);
    static void ProcessInitialInviteList(CDataStore* msg);
    static void ProcessInvite(CDataStore* msg, uint8_t* clearPending);
    static void ProcessInviteRemoved(CDataStore* msg, uint8_t* clearPending);
    static void ProcessInviteStatus(CDataStore* msg, uint8_t* clearPending);
    static void ProcessLockoutAdded(CDataStore* msg);
    static void ProcessLockoutRemoved(CDataStore* msg);
    static void ProcessLockoutUpdated(CDataStore* msg);
    static void ProcessInviteAlert(CDataStore* msg);
    static void ProcessInviteRemovedAlert(CDataStore* msg);
    static void ProcessInviteStatusAlert(CDataStore* msg);
    static void ProcessEventRemovedAlert(CDataStore* msg, uint8_t* clearPending);
    static void ProcessEventUpdatedAlert(CDataStore* msg, uint8_t* clearPending);
    static void ProcessInviteModeratorStatusAlert(CDataStore* msg, uint8_t* clearPending);
    static void ProcessInviteNotes(CDataStore* msg, uint8_t* clearPending);
    static void ProcessInviteNotesAlert(CDataStore* msg);
    static void UpdatePendingInvites(CDataStore* msg);
    static int32_t m_absMonth;
    static int32_t m_absYear;
    static int32_t m_absMonthDays;
    static CalendarEventSelection m_selectedEvent;
    static CalendarEventSelection m_contextEvent;
    static int32_t GetDaysInMonth(int32_t month, int32_t year, int32_t* days, int32_t* weekDay);
    static int32_t GetMonth(int32_t offset, int32_t* month, int32_t* year, int32_t* days, int32_t* weekDay);
    static int32_t ClampAbsMonthDay(int32_t month, int32_t year, int32_t* outMonth, int32_t* outYear, int32_t* days, int32_t* weekDay);
    static bool ComputeEventIndex(const CalendarEventSelection* selection, int32_t* monthOffset, int32_t* day, int32_t* index);
    static void CalculateEvents(bool a1);
};

int32_t Script_CalendarGetMaxDate(lua_State* L);
int32_t Script_CalendarGetMinHistoryDate(lua_State* L);
int32_t Script_CalendarGetMaxCreateDate(lua_State* L);
int32_t Script_CalendarGetAbsMonth(lua_State* L);
int32_t Script_CalendarSetMonth(lua_State* L);
int32_t Script_CalendarGetNumDayEvents(lua_State* L);
int32_t Script_CalendarGetDayEvent(lua_State* L);
int32_t Script_CalendarGetDayEventSequenceInfo(lua_State* L);
int32_t Script_CalendarGetFirstPendingInvite(lua_State* L);
int32_t Script_CalendarOpenEvent(lua_State* L);
int32_t Script_CalendarGetEventInfo(lua_State* L);
int32_t Script_CalendarGetHolidayInfo(lua_State* L);
int32_t Script_CalendarGetRaidInfo(lua_State* L);
int32_t Script_CalendarEventGetNumInvites(lua_State* L);
int32_t Script_CalendarEventGetInvite(lua_State* L);
int32_t Script_CalendarEventGetInviteResponseTime(lua_State* L);
int32_t Script_CalendarAddEvent(lua_State* L);
int32_t Script_CalendarNewEvent(lua_State* L);
int32_t Script_CalendarMassInviteGuild(lua_State* L);
int32_t Script_CalendarMassInviteArenaTeam(lua_State* L);
int32_t Script_CalendarNewGuildAnnouncement(lua_State* L);
int32_t Script_CalendarNewGuildEvent(lua_State* L);
int32_t Script_CalendarUpdateEvent(lua_State* L);
int32_t Script_CalendarRemoveEvent(lua_State* L);
int32_t Script_CalendarEventSelectInvite(lua_State* L);
int32_t Script_CalendarEventGetSelectedInvite(lua_State* L);
int32_t Script_CalendarContextSelectEvent(lua_State* L);
int32_t Script_CalendarContextDeselectEvent(lua_State* L);
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
void CalendarUnregisterScriptFunctions();
const char* GetHolidayName(int32_t holidayId);
const char* GetEventTypeString(uint32_t type);
const char* GetEventRepeatString(uint32_t repeat);

extern const char* s_weekdayNames[7];

#endif
