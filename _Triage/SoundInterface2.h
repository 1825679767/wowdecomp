#ifndef _TRIAGE_SOUNDINTERFACE2_H
#define _TRIAGE_SOUNDINTERFACE2_H

#include <cstddef>
#include <cstdint>

class SI3 {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[204];
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint8_t unk_00D4[2044];
    /* 08D0 */ uint32_t unk_08D0;
    /* 08D4 */ uint8_t unk_08D4[1676];
    /* 0F60 */ uint32_t unk_0F60;
    /* 0F64 */ uint8_t unk_0F64[164];
    /* 1008 */ uint32_t unk_1008[1026];
};

#endif
