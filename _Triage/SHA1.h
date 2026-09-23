#ifndef _TRIAGE_SHA1_H
#define _TRIAGE_SHA1_H

#include <cstddef>
#include <cstdint>

class SHA1 {
    public:
    /* 0000 */ uint64_t m_size;
    /* 0008 */ uint32_t m_hash [5];
    /* 001C */ uint8_t m_data [64];
    /* 005C */ uint8_t unk_005C[4];
};

template <class T>
struct HMAC {
    /* 0000 */ T m_hash;
    /* 0060 */ uint8_t m_innerPad [64];
    /* 00A0 */ uint8_t m_outerPad [64];
};

#endif
