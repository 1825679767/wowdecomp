#ifndef WOW_SOURCE_UI_TOTEMINFO_H
#define WOW_SOURCE_UI_TOTEMINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class TotemInfo {
    public:
    /* 0000 */ int32_t m_slot;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ const char* m_name;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
};

#endif
