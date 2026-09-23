#ifndef _TRIAGE_CLIENTLINK_H
#define _TRIAGE_CLIENTLINK_H

#include <cstddef>
#include <cstdint>

class ClientLink {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[144];
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint8_t unk_00A0[4];
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint8_t unk_00A8[292];
    /* 01CC */ uint32_t unk_01CC;
    /* 01D0 */ uint8_t unk_01D0[1280];
    /* 06D0 */ uint32_t unk_06D0;
    /* 06D4 */ uint32_t unk_06D4;
    /* 06D8 */ uint32_t unk_06D8;
    /* 06DC */ uint32_t unk_06DC;
};

#endif
