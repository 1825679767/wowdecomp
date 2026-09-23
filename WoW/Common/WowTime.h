#ifndef WOW_COMMON_WOWTIME_H
#define WOW_COMMON_WOWTIME_H

#include <cstddef>
#include <cstdint>

class WowTime {
    public:
    /* 0000 */ int32_t m_minute;
    /* 0004 */ int32_t m_hour;
    /* 0008 */ int32_t m_weekDay;
    /* 000C */ int32_t m_monthDay;
    /* 0010 */ int32_t m_month;
    /* 0014 */ int32_t m_year;
    /* 0018 */ int32_t m_flags;
    /* 001C */ int32_t m_tzHours;
};

#endif
