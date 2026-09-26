#ifndef WOW_COMMON_COMPONENTCORE_TYPES_H
#define WOW_COMMON_COMPONENTCORE_TYPES_H

#include <cstddef>
#include <cstdint>

struct CharColorList;
class CharSectionsRec;

enum COMPONENT_SECTIONS {
    SECTION_ARM_UPPER = 0,
    SECTION_ARM_LOWER = 1,
    SECTION_HAND = 2,
    SECTION_TORSO_UPPER = 3,
    SECTION_TORSO_LOWER = 4,
    SECTION_LEG_UPPER = 5,
    SECTION_LEG_LOWER = 6,
    SECTION_FOOT = 7,
    SECTION_HEAD_UPPER = 8,
    SECTION_HEAD_LOWER = 9,
    NUM_COMPONENT_SECTIONS = 10,
};

struct CharColorList {
    /* 0000 */ int32_t m_numColors;
    /* 0004 */ CharSectionsRec** m_color;
};

struct CharVariationList {
    /* 0000 */ int32_t m_numVariations;
    /* 0004 */ CharColorList* m_variation;
};

struct TCTEXTUREINFO {
    /* 0000 */ uint16_t m_width;
    /* 0002 */ uint16_t m_height;
    /* 0004 */ uint8_t m_mipCount;
    /* 0005 */ uint8_t m_alphaSize;
    /* 0006 */ uint16_t m_flags;
};

#endif
