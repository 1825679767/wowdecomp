#include "CalendarEvent.h"
#include "WoW/Source/Ui/Calendar.h"

// STATIC: 0x00AD1684
static const char* s_eventRepeatStrings[4] = { "CALENDAR_REPEAT_NEVER", "CALENDAR_REPEAT_WEEKLY", "CALENDAR_REPEAT_BIWEEKLY", "CALENDAR_REPEAT_MONTHLY" };

// STATIC: 0x00AD1694
static const char* s_eventTypeStrings[5] = { "CALENDAR_TYPE_RAID", "CALENDAR_TYPE_DUNGEON", "CALENDAR_TYPE_PVP", "CALENDAR_TYPE_MEETING", "CALENDAR_TYPE_OTHER" };

// VERIFY: 4baec31b manual
// OFFSET: 0x005F1020
const char* GetEventTypeString(uint32_t type) {
    return type < 5 ? s_eventTypeStrings[type] : nullptr;
}

// VERIFY: 92b48cfb manual
// OFFSET: 0x005F1040
const char* GetEventRepeatString(uint32_t repeat) {
    return repeat < 4 ? s_eventRepeatStrings[repeat] : nullptr;
}
