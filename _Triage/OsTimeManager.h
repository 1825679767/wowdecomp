#ifndef _TRIAGE_OSTIMEMANAGER_H
#define _TRIAGE_OSTIMEMANAGER_H

#include <cstddef>
#include <cstdint>
#include "_Triage/Types.h"

class OsTimeManager {
    public:
    /* 0000 */ double m_scaleToMs;
    /* 0008 */ TimingMethod m_timingMethod;
    /* 000C */ uint32_t m_timingTestError;
    /* 0010 */ int64_t m_performanceFrequency;
    /* 0018 */ double m_timeBegin;
};

#endif
