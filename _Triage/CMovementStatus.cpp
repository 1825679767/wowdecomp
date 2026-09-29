#include "CMovementStatus.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Object/Types.h"

// VERIFY: be14d437 manual
// OFFSET: 0x004F4CA0
uint32_t CMovementStatus::Skip(CDataStore* msg) {
    uint32_t moveFlags = 0;
    msg->Get(moveFlags);

    uint16_t extraFlags;
    msg->Get(extraFlags);

    void* data;
    msg->GetDataInSitu(data, 20);

    uint32_t size = 0;

    if (moveFlags & MOVEMENTFLAG_ONTRANSPORT) {
        SmartGUID transport;
        *msg >> transport;
        size = (extraFlags & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT) ? 25 : 21;
    }

    if ((moveFlags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (extraFlags & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        size += 4;
    }

    size += 4;

    if (moveFlags & MOVEMENTFLAG_FALLING) {
        size += 16;
    }

    if (moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        size += 4;
    }

    msg->GetDataInSitu(data, size);
    return moveFlags;
}

// VERIFY: 36f32364 auto
// OFFSET: 0x004F4C50
CMovementStatus::CMovementStatus() {
    this->m_gameTime = 0;
    this->m_transportGuid = WOWGUID();
    this->m_moveFlags = 0;
    this->m_moveExtraFlags = 0;
    this->m_transportSeat = 0xFF;
    this->m_transportPosition.x = 0.0f;
    this->m_transportPosition.y = 0.0f;
    this->m_transportPosition.z = 0.0f;
    this->m_transportFacing = 0.0f;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_fallTime = 0;
    this->m_facing = 0.0f;
    this->m_pitch = 0.0f;
    this->m_zSpeed = 0.0f;
    this->m_sinAngle = 0.0f;
    this->m_cosAngle = 0.0f;
    this->m_transportTime = 0;
    this->m_xySpeed = 0.0f;
    this->m_splineElevation = 0.0f;
}

// VERIFY: 69fc2cd0 manual
// OFFSET: 0x004F4D40
CDataStore& operator>>(CDataStore& msg, CMovementStatus& status) {
    CDataStore& data = msg.Get(status.m_moveFlags).Get(status.m_moveExtraFlags).Get(status.m_gameTime);
    data.Get(status.m_position.x);
    data.Get(status.m_position.y);
    data.Get(status.m_position.z);
    data.Get(status.m_facing);

    if (status.m_moveFlags & MOVEMENTFLAG_ONTRANSPORT) {
        SmartGUID transport;
        CDataStore& transportData = msg >> transport;
        transportData.Get(status.m_transportPosition.x);
        transportData.Get(status.m_transportPosition.y);
        transportData.Get(status.m_transportPosition.z);
        transportData.Get(status.m_transportFacing).Get(status.m_transportTime).Get(status.m_transportSeat);

        if (status.m_moveExtraFlags & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT) {
            status.m_moveExtraFlags &= ~MOVEMENTFLAG2_INTERPOLATED_MOVEMENT;
            msg.Get(status.m_transportTime2);
        } else {
            status.m_transportTime2 = status.m_transportTime;
        }

        status.m_transportGuid = transport;
    } else {
        status.m_transportGuid = WOWGUID();
        status.m_transportSeat = 0xFF;
    }

    if ((status.m_moveFlags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (status.m_moveExtraFlags & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        msg.Get(status.m_pitch);
    } else {
        status.m_pitch = 0.0f;
    }

    msg.Get(status.m_fallTime);

    if (status.m_moveFlags & MOVEMENTFLAG_FALLING) {
        CDataStore& fall = msg.Get(status.m_zSpeed);
        fall.Get(status.m_sinAngle);
        fall.Get(status.m_cosAngle);
        fall.Get(status.m_xySpeed);
    }

    if (status.m_moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        msg.Get(status.m_splineElevation);
    }

    return msg;
}
