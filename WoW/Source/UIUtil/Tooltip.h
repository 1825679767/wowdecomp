#ifndef WOW_SOURCE_UIUTIL_TOOLTIP_H
#define WOW_SOURCE_UIUTIL_TOOLTIP_H

#include <cstddef>
#include <cstdint>

class CGTooltip;

class CGItemStatsSummary {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[28];
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[180];
    /* 00E4 */ uint32_t unk_00E4;
    /* 00E8 */ uint8_t unk_00E8[64];
    /* 0128 */ uint32_t unk_0128;
};

class CSimpleUI {
    public:
    /* 0000 */ uint8_t unk_0000[720];
    /* 02D0 */ uint32_t unk_02D0;
    /* 02D4 */ uint8_t unk_02D4[84];
    /* 0328 */ uint32_t unk_0328;
    /* 032C */ uint32_t unk_032C;
    /* 0330 */ uint8_t unk_0330[40];
    /* 0358 */ uint32_t unk_0358;
    /* 035C */ uint32_t unk_035C;
    /* 0360 */ uint8_t unk_0360[396];
    /* 04EC */ uint32_t unk_04EC;
};

#endif
