#ifndef WOW_SOURCE_GAME_GAMETIME_H
#define WOW_SOURCE_GAME_GAMETIME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Common/WowTime.h"

class CGameTime : public WowTime {
    public:
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t m_minuteOffset;
    /* 0028 */ int32_t m_dayOffset;
    /* 002C */ int32_t m_minuteTicks;
    /* 0030 */ float m_minutesPerSecond;
    /* 0034 */ float m_fracMinutes;
    /* 0038 */ int32_t m_elapsedMinutes;
    /* 003C */ uint32_t m_lastTickMs;
    /* 0040 */ uint8_t m_running;
    /* 0041 */ uint8_t unk_0041[3];
    /* 0044 */ float m_baseMinuteOfDay;
    /* 0048 */ float unk_0048;
    /* 004C */ float unk_004C;

    float GameTimeGetDayProgression();
    bool IsDaytime();
};

extern CGameTime g_clientGameTime;

#endif
