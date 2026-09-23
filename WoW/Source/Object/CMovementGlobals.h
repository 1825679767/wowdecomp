#ifndef WOW_SOURCE_OBJECT_CMOVEMENTGLOBALS_H
#define WOW_SOURCE_OBJECT_CMOVEMENTGLOBALS_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Storm/H/stpl.h"

struct CMovement_C;

class CMovementGlobals {
    public:
    /* 0000 */ char unk_0000 [260];
    /* 0104 */ uint32_t unk_0104;
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint32_t unk_0114;
    /* 0118 */ TSList<CMovement_C> m_units;
    /* 0124 */ uint32_t m_flags;
    /* 0128 */ uint32_t m_lastUpdateTime;
    /* 012C */ uint32_t m_idleTime;
    /* 0130 */ uint32_t m_transportTime;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ uint32_t m_transportTime2;
    /* 013C */ uint32_t m_timeCounterMaybe;
};

#endif
