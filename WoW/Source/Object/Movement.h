#ifndef WOW_SOURCE_OBJECT_MOVEMENT_H
#define WOW_SOURCE_OBJECT_MOVEMENT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/Object/MovementShared.h"

struct CGUnit_C;

class CMovement_C : public CMovementShared {
    public:
    /* 00C4 */ uint32_t m_gameTime;
    /* 00C8 */ float m_collisionRadius;
    /* 00CC */ float m_collisionHeight;
    /* 00D0 */ float m_stepUpHeight;
    /* 00D4 */ C3Vector m_interpolationPos;
    /* 00E0 */ float m_interpolationFacing;
    /* 00E4 */ float m_interpolationPitch;
    /* 00E8 */ int16_t m_timeSkewSamples [32];
    /* 0128 */ uint32_t m_timeSkewIndex;
    /* 012C */ uint32_t m_timeSkew;
    /* 0130 */ uint32_t m_interpolation;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ TSList<void> m_moveQueue;
    /* 0144 */ CGUnit_C* m_unit;
};

struct ClipPolygon {
    /* 0000 */ C3Vector m_v [15];
    /* 00B4 */ int32_t m_flags [15];
    /* 00F0 */ int32_t m_count;
};

struct MoveState {
    /* 0000 */ C3Vector m_anchorPos;
    /* 000C */ float m_anchorFacing;
    /* 0010 */ float m_anchorPitch;
    /* 0014 */ uint32_t m_anchorElapsedMs;
    /* 0018 */ int32_t m_fallTimeMs;
    /* 001C */ C3Vector m_moveDir;
    /* 0028 */ C2Vector m_moveDir2D;
    /* 0030 */ uint32_t m_flags;
    /* 0034 */ float m_currentSpeed;
};

class CPlayerMoveQueue {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[4];
    /* 0008 */ uint32_t unk_0008;
};

#endif
