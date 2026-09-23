#ifndef _TRIAGE_PASSBOX4_H
#define _TRIAGE_PASSBOX4_H

#include <cstddef>
#include <cstdint>

class PassBox4 {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ uint8_t unk_0008 [28];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ uint8_t unk_0028 [4];
    /* 002C */ char unk_002C;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ int32_t unk_0030;
};

#endif
