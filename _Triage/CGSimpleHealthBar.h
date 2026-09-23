#ifndef _TRIAGE_CGSIMPLEHEALTHBAR_H
#define _TRIAGE_CGSIMPLEHEALTHBAR_H

#include <cstddef>
#include <cstdint>

class CGSimpleHealthBar {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[4];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[166];
    /* 00B2 */ uint8_t unk_00B2;
    /* 00B3 */ uint8_t unk_00B3;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ uint8_t unk_00B8[484];
    /* 029C */ uint32_t unk_029C[11];
    /* 02C8 */ uint32_t unk_02C8;
    /* 02CC */ uint32_t unk_02CC;
    /* 02D0 */ uint8_t unk_02D0[616];
};

#endif
