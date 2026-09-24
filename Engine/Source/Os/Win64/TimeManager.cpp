#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/TimeManager.h"

// STATIC: 0x00D4159C
static OsTimeManager* s_OsTimeMgr;

// OFFSET: 0x0086ADC0
uint64_t OsTimeManager::Snapshot() {
    switch (this->m_timingMethod) {
        case SystemMethod1:
            break;

        case SystemMethod2: {
            LARGE_INTEGER count;
            QueryPerformanceCounter(&count);
            return static_cast<int64_t>(static_cast<double>(count.QuadPart) * this->m_scaleToMs + this->m_timeBegin);
        }

        default:
            break;
    }

    int64_t ticks = GetTickCount();
    return static_cast<int64_t>(static_cast<double>(ticks) * this->m_scaleToMs + this->m_timeBegin);
}

// OFFSET: 0x0086AE20
uint32_t OsGetAsyncTimeMs() {
    return static_cast<uint32_t>(s_OsTimeMgr->Snapshot());
}

// OFFSET: 0x0086B280
void OsSleep(uint32_t milliseconds) {
    Sleep(milliseconds);
}

#endif
