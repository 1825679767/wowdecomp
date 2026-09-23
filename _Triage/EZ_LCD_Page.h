#ifndef _TRIAGE_EZ_LCD_PAGE_H
#define _TRIAGE_EZ_LCD_PAGE_H

#include <cstddef>
#include <cstdint>

class CEzLcdPage {
    public:
    /* 0000 */ uint8_t unk_0000 [20];
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ uint8_t unk_0018 [12];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ uint8_t unk_002C [4];
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ uint8_t unk_0034 [20];
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ uint8_t unk_0060 [84];
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;
};

#endif
