#ifndef STORM_SOURCE_W32_STHREAD_H
#define STORM_SOURCE_W32_STHREAD_H

#include <cstddef>
#include <cstdint>
#include "Storm/Source/W32/SLock.h"

class SThread : public SSyncObject {
    public:
    int32_t SetPriority(int32_t priority);
};

#endif
