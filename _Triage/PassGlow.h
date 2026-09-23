#ifndef _TRIAGE_PASSGLOW_H
#define _TRIAGE_PASSGLOW_H

#include <cstddef>
#include <cstdint>

class PassGlow {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ uint8_t m_gapC [24];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ uint8_t unk_0028 [4];
    /* 002C */ char unk_002C;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
};

#endif
