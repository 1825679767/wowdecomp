#ifndef WOW_SOURCE_OBJECT_MOVEMENTSHARED_H
#define WOW_SOURCE_OBJECT_MOVEMENTSHARED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/Object/Passenger.h"

struct CMoveSpline;

class CMovementShared : public CPassenger {
    public:
    /* 0030 */ TSLink<void> m_globalUnitsLink;
    /* 0038 */ C3Vector m_groundNormal;
    /* 0044 */ uint32_t m_flags;
    /* 0048 */ uint32_t m_flags2;
    /* 004C */ C3Vector m_anchorPos;
    /* 0058 */ float m_anchorFacing;
    /* 005C */ float m_anchorPitch;
    /* 0060 */ uint32_t m_anchorElapsedMs;
    /* 0064 */ C3Vector m_moveDir;
    /* 0070 */ C2Vector m_moveDir2D;
    /* 0078 */ float m_pitchCos;
    /* 007C */ float m_pitchSin;
    /* 0080 */ uint32_t m_fallTimeMs;
    /* 0084 */ float m_fallStartZ;
    /* 0088 */ float m_stepUpStartZ;
    /* 008C */ float m_currentSpeed;
    /* 0090 */ float m_walkSpeed;
    /* 0094 */ float m_runSpeed;
    /* 0098 */ float m_runBackSpeed;
    /* 009C */ float m_swimSpeed;
    /* 00A0 */ float m_swimBackSpeed;
    /* 00A4 */ float m_flightSpeed;
    /* 00A8 */ float m_flightBackSpeed;
    /* 00AC */ float m_turnRate;
    /* 00B0 */ float m_pitchRate;
    /* 00B4 */ float m_hoverHeight;
    /* 00B8 */ float m_fallVelocity;
    /* 00BC */ CMoveSpline* m_spline;
    /* 00C0 */ uint32_t m_statusTimeMs;
};

#endif
