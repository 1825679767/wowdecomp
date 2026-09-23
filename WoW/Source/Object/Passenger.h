#ifndef WOW_SOURCE_OBJECT_PASSENGER_H
#define WOW_SOURCE_OBJECT_PASSENGER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

class CPassenger {
    public:
    /* 0000 */ TSLink<void> m_link;
    /* 0008 */ WGUID m_transportGuid;
    /* 0010 */ C3Vector m_position;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ float m_facing;
    /* 0024 */ float m_pitch;
    /* 0028 */ WGUID* m_guid2;
    /* 002C */ uint32_t unk_002C;
};

#endif
