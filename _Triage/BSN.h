#ifndef _TRIAGE_BSN_H
#define _TRIAGE_BSN_H

#include <cstddef>
#include <cstdint>

class BNetStr2 {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[28];
    /* 0024 */ uint32_t unk_0024;
};

#endif
