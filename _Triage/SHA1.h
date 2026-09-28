#ifndef _TRIAGE_SHA1_H
#define _TRIAGE_SHA1_H

#include <cstddef>
#include <cstdint>
#include <cstring>

class SHA1 {
    public:
    struct Digest {
        /* 0000 */ uint8_t m_data[20];
    };

    /* 0000 */ uint64_t m_size;
    /* 0008 */ uint32_t m_hash [5];
    /* 001C */ uint8_t m_data [64];
    /* 005C */ uint8_t unk_005C[4];

    void Prepare();
    void Process(const void* data, uint32_t length);
    void Finish(SHA1::Digest& digest);
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

    void Finish(typename T::Digest& digest);
    void Init(const uint8_t* key, uint32_t length);
    void Process(const void* data, uint32_t length);
};

void Process(uint32_t* hash, const uint8_t* data);
void SHA1_Final(uint8_t* digest, SHA1_CONTEXT* context);
void SHA1_Init(SHA1_CONTEXT* context);
uint8_t* SHA1_InterleaveHash(uint8_t* out, const uint8_t* in, uint32_t length);
void SHA1_Transform(uint32_t* state, uint8_t* buffer);
void SHA1_Update(SHA1_CONTEXT* context, const uint8_t* data, uint32_t length);

// VERIFY: 7a1b2c77 auto
// OFFSET: 0x004668A0
template <class T>
void HMAC<T>::Init(const uint8_t* key, uint32_t length) {
    memset(this->m_innerPad, 0x36, sizeof(this->m_innerPad));
    memset(this->m_outerPad, 0x5C, sizeof(this->m_outerPad));

    for (uint32_t i = 0; i < length; i++) {
        this->m_innerPad[i] ^= key[i];
        this->m_outerPad[i] ^= key[i];
    }

    this->m_hash.Prepare();
    this->m_hash.Process(this->m_innerPad, sizeof(this->m_innerPad));
}

// INLINED
template <class T>
void HMAC<T>::Finish(typename T::Digest& digest) {
    typename T::Digest inner;
    this->m_hash.Finish(inner);

    T outer;
    outer.Prepare();
    outer.Process(this->m_outerPad, sizeof(this->m_outerPad));
    outer.Process(&inner, sizeof(inner));
    outer.Finish(digest);
}

// INLINED
template <class T>
void HMAC<T>::Process(const void* data, uint32_t length) {
    this->m_hash.Process(data, length);
}

#endif
