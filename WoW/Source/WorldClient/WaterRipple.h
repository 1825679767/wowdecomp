#ifndef WOW_SOURCE_WORLDCLIENT_WATERRIPPLE_H
#define WOW_SOURCE_WORLDCLIENT_WATERRIPPLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CGxShader;

class CWaterRipple {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ uint8_t unk_000C[36];
    /* 0030 */ TSLink<CWaterRipple> m_link;
    /* 0038 */ TSLink<CWaterRipple> unk_0038;
    /* 0040 */ TSFixedArray<C3Vector> unk_0040;
    /* 004C */ uint32_t unk_004C;

    CWaterRipple();
    ~CWaterRipple();
};

class WaterRipple {
    public:
    static uint32_t s_nextPlayerRipple;
    static uint32_t s_nextUnitRipple;
    static CGxShader* s_psWaterRipples;
    static CGxShader* s_vsWaterRipples;

    static void Destroy();
    static void Initialize();
};

#endif
