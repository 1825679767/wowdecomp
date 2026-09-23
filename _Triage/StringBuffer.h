#ifndef _TRIAGE_STRINGBUFFER_H
#define _TRIAGE_STRINGBUFFER_H

#include <cstddef>
#include <cstdint>

class StringBuffer {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
};

#endif
