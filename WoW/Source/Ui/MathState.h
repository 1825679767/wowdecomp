#ifndef WOW_SOURCE_UI_MATHSTATE_H
#define WOW_SOURCE_UI_MATHSTATE_H

#include <cstddef>
#include <cstdint>

class MathState {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[128];
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint8_t unk_008C[256];
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint8_t unk_0190[256];
    /* 0290 */ uint32_t unk_0290;
    /* 0294 */ uint8_t unk_0294[132];
    /* 0318 */ uint32_t unk_0318;
};

#endif
