#ifndef _TRIAGE_CLCDOUTPUT_H
#define _TRIAGE_CLCDOUTPUT_H

#include <cstddef>
#include <cstdint>

class CLCDOutput {
    public:
    /* 0000 */ uint8_t unk_0000 [48];
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ uint8_t unk_0034 [20];
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ uint8_t unk_0068 [32];
    /* 0088 */ int32_t unk_0088;
    /* 008C */ int32_t unk_008C;
    /* 0090 */ uint8_t unk_0090 [4];
    /* 0094 */ int32_t unk_0094;
    /* 0098 */ int32_t unk_0098;
    /* 009C */ int32_t unk_009C;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ int32_t unk_00A4;
};

#endif
