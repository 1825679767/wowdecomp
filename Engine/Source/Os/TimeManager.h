#ifndef ENGINE_SOURCE_OS_TIMEMANAGER_H
#define ENGINE_SOURCE_OS_TIMEMANAGER_H

#include <cstdint>

class OsTimeManager {
    public:
    enum TimingMethod {
        NotSet = 0,
        SystemMethod1 = 1,
        SystemMethod2 = 2,
    };

    /* 0000 */ double m_scaleToMs;
    /* 0008 */ OsTimeManager::TimingMethod m_timingMethod;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint64_t unk_0010;
    /* 0018 */ double m_timeBegin;

    uint64_t Snapshot();
};

uint32_t OsGetAsyncTimeMs();
void OsSleep(uint32_t milliseconds);

#endif
