#include "GameTime.h"
#include "Engine/Source/Os/TimeManager.h"

// STATIC: 0x00D37F98
CGameTime g_clientGameTime;

// VERIFY: c77f987f auto
// OFFSET: 0x0076CFF0
float CGameTime::GameTimeGetDayProgression() {
    uint32_t elapsed;

    if (this->m_running) {
        OsGetAsyncTimeMs();
        elapsed = OsGetAsyncTimeMs() - this->m_lastTickMs;
    } else {
        elapsed = 0;
    }

    float minutes = elapsed * 0.001f * this->m_minutesPerSecond + this->m_baseMinuteOfDay;

    while (minutes > 1440.0f) {
        minutes -= 1440.0f;
    }

    return minutes * (1.0f / 1440.0f);
}

// VERIFY: ad8714a6 auto
// OFFSET: 0x0076D600
CGameTime::CGameTime() {
    this->m_minutesPerSecond = 1.0f / 60.0f;
    this->m_fracMinutes = 0.0f;
    this->unk_0020 = 0;
    this->m_baseMinuteOfDay = 0.0f;
    this->m_minuteOffset = 0;
    this->unk_0048 = 0.0f;
    this->m_dayOffset = 0;
    this->unk_004C = 0.0f;
    this->m_minuteTicks = 0;
    this->m_elapsedMinutes = 0;
    this->m_lastTickMs = 0;
    this->m_running = 0;
}
