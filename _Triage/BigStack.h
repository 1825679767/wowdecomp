#ifndef _TRIAGE_BIGSTACK_H
#define _TRIAGE_BIGSTACK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class BigBuffer {
    public:
    /* 0000 */ TSGrowableArray<uint32_t> m_data;
    /* 0010 */ uint32_t m_offset;
};

class BigStack {
    public:
    /* 0000 */ BigBuffer m_buffer [16];
    /* 0140 */ uint32_t m_used;
};

#endif
