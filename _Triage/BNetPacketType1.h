#ifndef _TRIAGE_BNETPACKETTYPE1_H
#define _TRIAGE_BNETPACKETTYPE1_H

#include <cstddef>
#include <cstdint>

class BNetPacketType1 {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[4];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint8_t unk_0020;
};

#endif
