#ifndef ENGINE_SOURCE_TEMPEST_C3VECTOR_H
#define ENGINE_SOURCE_TEMPEST_C3VECTOR_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Glue/Types.h"

class C3Vector {
    public:
    /* 0000 */ float m_x;
    /* 0004 */ float m_y;
    /* 0008 */ float m_z;
};

struct C3Segment {
    /* 0000 */ C3Vector m_b;
    /* 000C */ C3Vector m_t;
};

class CRay {
    public:
    /* 0000 */ C3Vector m_origin;
    /* 000C */ C3Vector m_dir;
};

struct CharacterInfo {
    /* 0000 */ uint64_t m_guid;
    /* 0008 */ char m_name [48];
    /* 0038 */ uint32_t m_mapID;
    /* 003C */ uint32_t m_zoneID;
    /* 0040 */ uint32_t m_guildID;
    /* 0044 */ C3Vector m_position;
    /* 0050 */ uint32_t m_inventoryItemDisplayId [23];
    /* 00AC */ uint32_t m_inventoryItemType [23];
    /* 0108 */ uint32_t m_inventoryItemEnchantmentId [23];
    /* 0164 */ uint32_t m_petDisplayId;
    /* 0168 */ uint32_t m_petLevel;
    /* 016C */ uint32_t m_petFamilyId;
    /* 0170 */ CharEnumFlags m_flags1;
    /* 0174 */ uint32_t m_flags2;
    /* 0178 */ uint8_t m_raceId;
    /* 0179 */ uint8_t m_classId;
    /* 017A */ uint8_t m_genderId;
    /* 017B */ uint8_t m_skinId;
    /* 017C */ uint8_t m_facialHairId;
    /* 017D */ uint8_t m_hairStyleId;
    /* 017E */ uint8_t m_faceId;
    /* 017F */ uint8_t m_hairColorId;
    /* 0180 */ uint8_t m_level;
    /* 0181 */ uint8_t m_firstLogin;
    /* 0182 */ uint8_t unk_0182;
    /* 0183 */ uint8_t unk_0183;
    /* 0184 */ uint8_t unk_0184;
    /* 0185 */ uint8_t unk_0185;
    /* 0186 */ uint8_t unk_0186;
    /* 0187 */ uint8_t unk_0187;
};

#endif
