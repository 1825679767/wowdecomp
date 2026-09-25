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

struct SHA1_CONTEXT {
    /* 0000 */ uint32_t m_state[5];
    /* 0014 */ uint32_t m_count[2];
    /* 001C */ uint8_t m_buffer[64];
};

template <class T>
struct HMAC {
    /* 0000 */ T m_hash;
    /* 0060 */ uint8_t m_innerPad [64];
    /* 00A0 */ uint8_t m_outerPad [64];
};

void SHA1_Final(uint8_t* digest, SHA1_CONTEXT* context);
void SHA1_Init(SHA1_CONTEXT* context);
uint8_t* SHA1_InterleaveHash(uint8_t* out, const uint8_t* in, uint32_t length);
void SHA1_Transform(uint32_t* state, uint8_t* buffer);
void SHA1_Update(SHA1_CONTEXT* context, const uint8_t* data, uint32_t length);

#endif
