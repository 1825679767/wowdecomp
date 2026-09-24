#ifndef _TRIAGE_CMOVESPLINE_H
#define _TRIAGE_CMOVESPLINE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"

class CMoveSpline {
    public:
    /* 0000 */ TSLink<void> unk_0000;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t m_face[12];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t m_flags;
    /* 0024 */ uint32_t m_timePassed;
    /* 0028 */ uint32_t m_start;
    /* 002C */ uint32_t m_duration;
    /* 0030 */ uint32_t m_id;
    /* 0034 */ C3Spline_CatmullRom m_spline;
    /* 01F8 */ C3Vector m_finalDestination;
    /* 0204 */ float m_durationMod;
    /* 0208 */ float m_durationModNext;
    /* 020C */ float m_verticalAcceleration;
    /* 0210 */ uint32_t m_effectStartTime;
    /* 0214 */ TSLink<void> unk_0214;
    /* 021C */ uint32_t unk_021C;
};

#endif
