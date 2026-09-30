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

// VERIFY: c5dfac8a manual
// OFFSET: 0x0076CFA0
float CGameTime::GameTimeSetMinutesPerSecond(float minutesPerSecond) {
    float old = this->m_minutesPerSecond;

    if (minutesPerSecond < 1.0f / 60.0f) {
        this->m_minutesPerSecond = 1.0f / 60.0f;
    } else if (minutesPerSecond > 60.0f) {
        this->m_minutesPerSecond = 60.0f;
    } else {
        this->m_minutesPerSecond = minutesPerSecond;
    }

    return old;
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

// VERIFY: d1c1b754 auto
// OFFSET: 0x004C9850
bool CGameTime::IsDaytime() {
    CGameTime time;
    time.SetHourAndMinutes(this->GetHourAndMinutes());
    time.SetDate(0, 0, 0);

    uint32_t value;
    WowTime end;
    WowTime::WowEncodeTime(value, 0, 21, -1, -1, -1, -1, 0);
    WowTime::WowDecodeTime(value, &end);
    end.m_tzHours = 0;

    WowTime start;
    WowTime::WowEncodeTime(value, 30, 5, -1, -1, -1, -1, 0);
    WowTime::WowDecodeTime(value, &start);
    start.m_tzHours = 0;

    return time.InRange(start, end);
}
