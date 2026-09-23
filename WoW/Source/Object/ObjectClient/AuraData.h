#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_AURADATA_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_AURADATA_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class AuraData {
    public:
    /* 0000 */ WGUID m_creator;
    /* 0008 */ int32_t m_spellId;
    /* 000C */ uint8_t m_flags;
    /* 000D */ uint8_t m_level;
    /* 000E */ uint8_t m_stackCount;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ int32_t m_duration;
    /* 0014 */ int32_t m_endTime;
};

#endif
