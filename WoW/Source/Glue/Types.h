#ifndef WOW_SOURCE_GLUE_TYPES_H
#define WOW_SOURCE_GLUE_TYPES_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include <windows.h>

enum CHAR_CUSTOMIZATION_TYPE {
    CHAR_CUSTOMIZATION_SKIN = 0,
    CHAR_CUSTOMIZATION_FACE = 1,
    CHAR_CUSTOMIZATION_HAIR_STYLE = 2,
    CHAR_CUSTOMIZATION_HAIR_COLOR = 3,
    CHAR_CUSTOMIZATION_FACIAL_FEATURE = 4,
};

enum CharEnumFlags : uint32_t {
    CHARACTER_FLAG_NONE = 0,
    CHARACTER_FLAG_UNK1 = 1,
    CHARACTER_FLAG_UNK2 = 2,
    CHARACTER_LOCKED_FOR_TRANSFER = 4,
    CHARACTER_FLAG_UNK4 = 8,
    CHARACTER_FLAG_UNK5 = 16,
    CHARACTER_FLAG_UNK6 = 32,
    CHARACTER_FLAG_UNK7 = 64,
    CHARACTER_FLAG_UNK8 = 128,
    CHARACTER_FLAG_UNK9 = 0x100,
    CHARACTER_FLAG_UNK10 = 0x200,
    CHARACTER_FLAG_HIDE_HELM = 0x400,
    CHARACTER_FLAG_HIDE_CLOAK = 0x800,
    CHARACTER_FLAG_UNK13 = 0x1000,
    CHARACTER_FLAG_GHOST = 0x2000,
    CHARACTER_FLAG_RENAME = 0x4000,
    CHARACTER_FLAG_UNK16 = 0x8000,
    CHARACTER_FLAG_UNK17 = 0x10000,
    CHARACTER_FLAG_UNK18 = 0x20000,
    CHARACTER_FLAG_UNK19 = 0x40000,
    CHARACTER_FLAG_UNK20 = 0x80000,
    CHARACTER_FLAG_UNK21 = 0x100000,
    CHARACTER_FLAG_UNK22 = 0x200000,
    CHARACTER_FLAG_UNK23 = 0x400000,
    CHARACTER_FLAG_UNK24 = 0x800000,
    CHARACTER_FLAG_LOCKED_BY_BILLING = 0x1000000,
    CHARACTER_FLAG_DECLINED = 0x2000000,
    CHARACTER_FLAG_UNK27 = 0x4000000,
    CHARACTER_FLAG_UNK28 = 0x8000000,
    CHARACTER_FLAG_UNK29 = 0x10000000,
    CHARACTER_FLAG_UNK30 = 0x20000000,
    CHARACTER_FLAG_UNK31 = 0x40000000,
    CHARACTER_FLAG_UNK32 = 0xFFFFFFFF80000000,
};

struct CharacterPreferences {
    /* 0000 */ uint32_t m_raceID;
    /* 0004 */ uint32_t m_sexID;
    /* 0008 */ uint32_t m_classId;
    /* 000C */ uint32_t m_hairColorId;
    /* 0010 */ uint32_t m_skinID;
    /* 0014 */ uint32_t m_faceId;
    /* 0018 */ uint32_t m_facialHairId;
    /* 001C */ uint32_t m_hairStyleId;
};

enum GLUE_IDLE_STATE {
    IDLE_NONE = 0,
    IDLE_LOGIN_SERVER_LOGIN = 1,
    IDLE_ACCOUNT_LOGIN = 2,
    IDLE_CHARACTER_LIST = 3,
    IDLE_REALM_LIST = 4,
    IDLE_CREATE_CHARACTER = 5,
    IDLE_DELETE_CHARACTER = 6,
    IDLE_7 = 7,
    IDLE_8 = 8,
    IDLE_9 = 9,
    IDLE_ENTER_WORLD = 10,
    IDLE_WORLD_LOGIN = 11,
    IDLE_12 = 12,
    IDLE_13 = 13,
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
