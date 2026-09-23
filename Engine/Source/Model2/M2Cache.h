#ifndef ENGINE_SOURCE_MODEL2_M2CACHE_H
#define ENGINE_SOURCE_MODEL2_M2CACHE_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"

struct CM2Shared;

class CM2Cache {
    public:
    /* 0000 */ uint32_t m_initialized;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ CM2Shared* m_shared [1021];
    /* 1004 */ uint32_t unk_1004;
    /* 1008 */ uint32_t unk_1008;
    /* 100C */ uint32_t unk_100C;
    /* 1010 */ uint32_t unk_1010;
    /* 1014 */ uint32_t unk_1014;
    /* 1018 */ uint32_t unk_1018;
    /* 101C */ uint32_t unk_101C;
    /* 1020 */ uint32_t unk_1020 [15];
    /* 105C */ uint32_t unk_105C;
    /* 1060 */ uint32_t unk_1060 [16];
    /* 10A0 */ TSList<void> unk_10A0;
};

#endif
