#ifndef WOW_SOURCE_OBJECT_PASSENGER_H
#define WOW_SOURCE_OBJECT_PASSENGER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

#pragma pack(push, 4)
class CPassenger {
    public:
    /* 0000 */ TSLink<void> m_link;
    /* 0008 */ WOWGUID m_transportGuid;
    /* 0010 */ C3Vector m_position;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ float m_facing;
    /* 0024 */ float m_pitch;
    /* 0028 */ const WOWGUID* m_ownerGuid;
    /* 002C */ uint8_t unk_002C;

    CPassenger(const WOWGUID& transportGuid, const C3Vector& position, const WOWGUID& ownerGuid);
    void PostInit(const C3Vector& position);
    void Reenable();
    float GetFacing(float facing) const;
    float GetRawFacing(float facing) const;
    C3Vector GetPosition(const C3Vector& position) const;
    C3Vector GetRawPosition(const C3Vector& position) const;
};
#pragma pack(pop)

#endif
