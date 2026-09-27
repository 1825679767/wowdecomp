#ifndef WOW_SOURCE_UIUTIL_ADDONS_H
#define WOW_SOURCE_UIUTIL_ADDONS_H

#include <cstddef>
#include <cstdint>

class UIADDON {
    public:
    /* 0000 */ uint8_t unk_0000[48];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint8_t unk_0034[4];
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint8_t unk_0044[4];
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint8_t unk_0054[4];
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint8_t unk_0064[4];
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[4];
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t unk_0080;
    /* 0084 */ uint8_t unk_0084[4];
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint8_t unk_0094[4];
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint8_t unk_00A4[4];
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint8_t unk_00AC[12];
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint8_t unk_00BC[4];
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
};

void DeleteInterfaceFiles();

#endif
