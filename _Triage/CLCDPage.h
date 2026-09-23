#ifndef _TRIAGE_CLCDPAGE_H
#define _TRIAGE_CLCDPAGE_H

#include <cstddef>
#include <cstdint>

class CLCDPage {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[72];
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint8_t unk_005C[88];
    /* 00B4 */ uint32_t unk_00B4;
};

#endif
