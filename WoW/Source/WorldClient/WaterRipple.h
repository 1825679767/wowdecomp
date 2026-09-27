#ifndef WOW_SOURCE_WORLDCLIENT_WATERRIPPLE_H
#define WOW_SOURCE_WORLDCLIENT_WATERRIPPLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CGxShader;

class CWaterRipple {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ uint8_t unk_000C[36];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;

    CWaterRipple();
};

class WaterRipple {
    public:
    static uint32_t s_nextPlayerRipple;
    static uint32_t s_nextUnitRipple;
    static CGxShader* s_psWaterRipples;
    static CGxShader* s_vsWaterRipples;

    static void Initialize();
};

#endif
