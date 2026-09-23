#ifndef _TRIAGE_SERVERLINK_H
#define _TRIAGE_SERVERLINK_H

#include <cstddef>
#include <cstdint>

class ServerLink {
    public:
    /* 0000 */ uint8_t unk_0000[148];
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint8_t unk_00A4[8];
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ uint8_t unk_00B8[172];
    /* 0164 */ uint32_t unk_0164;
    /* 0168 */ uint32_t unk_0168;
};

#endif
