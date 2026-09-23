#ifndef ENGINE_SOURCE_EVENT_CMOUSEEVENT_H
#define ENGINE_SOURCE_EVENT_CMOUSEEVENT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C2Vector.h"

class CMouseEvent {
    public:
    /* 0000 */ void* m_vtable;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_id;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ MouseEventData m_data;
};

#endif
