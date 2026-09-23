#ifndef _TRIAGE_BATTLENETFILEDOWNLOAD_H
#define _TRIAGE_BATTLENETFILEDOWNLOAD_H

#include <cstddef>
#include <cstdint>

class BattlenetFileDownload {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[63];
    /* 0043 */ uint8_t unk_0043;
    /* 0044 */ uint16_t unk_0044;
    /* 0046 */ uint8_t unk_0046;
    /* 0047 */ uint8_t unk_0047[2];
    /* 0049 */ uint8_t unk_0049;
    /* 004A */ uint8_t unk_004A;
    /* 004B */ uint8_t unk_004B;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[184];
    /* 0108 */ uint8_t unk_0108;
    /* 0109 */ uint8_t unk_0109[3];
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint8_t unk_0114;
    /* 0115 */ uint8_t unk_0115[11];
    /* 0120 */ uint32_t unk_0120;
    /* 0124 */ uint32_t unk_0124;
    /* 0128 */ uint32_t unk_0128;
    /* 012C */ uint32_t unk_012C;
};

#endif
