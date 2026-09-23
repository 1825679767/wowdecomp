#ifndef WOW_COMMON_TYPES_H
#define WOW_COMMON_TYPES_H

#include <cstddef>
#include <cstdint>

class CAaBspNode {
    public:
    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint16_t m_negChild;
    /* 0004 */ uint16_t m_posChild;
    /* 0006 */ uint16_t m_nFaces;
    /* 0008 */ uint32_t m_faceStart;
    /* 000C */ float m_planeDist;
};

enum EGxTexCommand {
    GxTex_Lock = 0,
    GxTex_Latch = 1,
    GxTex_Unlock = 2,
    GxTex_3 = 3,
    GxTexCommands_Last = 4,
};

enum EGxTexFormat {
    GxTex_Unknown = 0,
    GxTex_Abgr8888 = 1,
    GxTex_Argb8888 = 2,
    GxTex_Argb4444 = 3,
    GxTex_Argb1555 = 4,
    GxTex_Rgb565 = 5,
    GxTex_Dxt1 = 6,
    GxTex_Dxt3 = 7,
    GxTex_Dxt5 = 8,
    GxTex_Uv88 = 9,
    GxTex_Gr1616F = 10,
    GxTex_R32F = 11,
    GxTex_D24X8 = 12,
    GxTexFormats_Last = 13,
};

#endif
