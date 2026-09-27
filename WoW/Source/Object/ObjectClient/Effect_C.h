#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H

#include <cstddef>
#include <cstdint>

class CEffect {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001[3];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint8_t unk_0040;
    /* 0041 */ uint8_t unk_0041;
    /* 0042 */ uint16_t unk_0042;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint8_t unk_007C[4];
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint8_t unk_00BC[4];
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ uint32_t unk_00DC;
    /* 00E0 */ uint32_t unk_00E0;
    /* 00E4 */ uint32_t unk_00E4;
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ int32_t m_refCount;
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ uint32_t unk_00FC;
    /* 0100 */ uint32_t unk_0100;
    /* 0104 */ uint32_t unk_0104;
    /* 0108 */ uint32_t unk_0108;

    static void Destroy();

    ~CEffect();
};

int32_t SpecialUnitEffectsInitialize();

#endif
