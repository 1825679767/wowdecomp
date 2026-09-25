#include <cstdint>
#include <cstring>
#include <malloc.h>
#include "SHA1.h"

#define SHA1_ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define SHA1_BLK0(i) (block[i] = (SHA1_ROL(block[i], 24) & 0xFF00FF00) | (SHA1_ROL(block[i], 8) & 0x00FF00FF))
#define SHA1_BLK(i) (block[i & 15] = SHA1_ROL(block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^ block[i & 15], 1))
#define SHA1_R0(v, w, x, y, z, i) z += ((w & (x ^ y)) ^ y) + SHA1_BLK0(i) + 0x5A827999 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R1(v, w, x, y, z, i) z += ((w & (x ^ y)) ^ y) + SHA1_BLK(i) + 0x5A827999 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R2(v, w, x, y, z, i) z += (w ^ x ^ y) + SHA1_BLK(i) + 0x6ED9EBA1 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R3(v, w, x, y, z, i) z += (((w | x) & y) | (w & x)) + SHA1_BLK(i) + 0x8F1BBCDC + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);
#define SHA1_R4(v, w, x, y, z, i) z += (w ^ x ^ y) + SHA1_BLK(i) + 0xCA62C1D6 + SHA1_ROL(v, 5); w = SHA1_ROL(w, 30);

// VERIFY: 5a235895 manual
// OFFSET: 0x006CA330
void SHA1_Transform(uint32_t* state, uint8_t* buffer) {
    uint32_t* block = reinterpret_cast<uint32_t*>(buffer);

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t e = state[4];

    SHA1_R0(a, b, c, d, e, 0) SHA1_R0(e, a, b, c, d, 1) SHA1_R0(d, e, a, b, c, 2) SHA1_R0(c, d, e, a, b, 3)
    SHA1_R0(b, c, d, e, a, 4) SHA1_R0(a, b, c, d, e, 5) SHA1_R0(e, a, b, c, d, 6) SHA1_R0(d, e, a, b, c, 7)
    SHA1_R0(c, d, e, a, b, 8) SHA1_R0(b, c, d, e, a, 9) SHA1_R0(a, b, c, d, e, 10) SHA1_R0(e, a, b, c, d, 11)
    SHA1_R0(d, e, a, b, c, 12) SHA1_R0(c, d, e, a, b, 13) SHA1_R0(b, c, d, e, a, 14) SHA1_R0(a, b, c, d, e, 15)
    SHA1_R1(e, a, b, c, d, 16) SHA1_R1(d, e, a, b, c, 17) SHA1_R1(c, d, e, a, b, 18) SHA1_R1(b, c, d, e, a, 19)
    SHA1_R2(a, b, c, d, e, 20) SHA1_R2(e, a, b, c, d, 21) SHA1_R2(d, e, a, b, c, 22) SHA1_R2(c, d, e, a, b, 23)
    SHA1_R2(b, c, d, e, a, 24) SHA1_R2(a, b, c, d, e, 25) SHA1_R2(e, a, b, c, d, 26) SHA1_R2(d, e, a, b, c, 27)
    SHA1_R2(c, d, e, a, b, 28) SHA1_R2(b, c, d, e, a, 29) SHA1_R2(a, b, c, d, e, 30) SHA1_R2(e, a, b, c, d, 31)
    SHA1_R2(d, e, a, b, c, 32) SHA1_R2(c, d, e, a, b, 33) SHA1_R2(b, c, d, e, a, 34) SHA1_R2(a, b, c, d, e, 35)
    SHA1_R2(e, a, b, c, d, 36) SHA1_R2(d, e, a, b, c, 37) SHA1_R2(c, d, e, a, b, 38) SHA1_R2(b, c, d, e, a, 39)
    SHA1_R3(a, b, c, d, e, 40) SHA1_R3(e, a, b, c, d, 41) SHA1_R3(d, e, a, b, c, 42) SHA1_R3(c, d, e, a, b, 43)
    SHA1_R3(b, c, d, e, a, 44) SHA1_R3(a, b, c, d, e, 45) SHA1_R3(e, a, b, c, d, 46) SHA1_R3(d, e, a, b, c, 47)
    SHA1_R3(c, d, e, a, b, 48) SHA1_R3(b, c, d, e, a, 49) SHA1_R3(a, b, c, d, e, 50) SHA1_R3(e, a, b, c, d, 51)
    SHA1_R3(d, e, a, b, c, 52) SHA1_R3(c, d, e, a, b, 53) SHA1_R3(b, c, d, e, a, 54) SHA1_R3(a, b, c, d, e, 55)
    SHA1_R3(e, a, b, c, d, 56) SHA1_R3(d, e, a, b, c, 57) SHA1_R3(c, d, e, a, b, 58) SHA1_R3(b, c, d, e, a, 59)
    SHA1_R4(a, b, c, d, e, 60) SHA1_R4(e, a, b, c, d, 61) SHA1_R4(d, e, a, b, c, 62) SHA1_R4(c, d, e, a, b, 63)
    SHA1_R4(b, c, d, e, a, 64) SHA1_R4(a, b, c, d, e, 65) SHA1_R4(e, a, b, c, d, 66) SHA1_R4(d, e, a, b, c, 67)
    SHA1_R4(c, d, e, a, b, 68) SHA1_R4(b, c, d, e, a, 69) SHA1_R4(a, b, c, d, e, 70) SHA1_R4(e, a, b, c, d, 71)
    SHA1_R4(d, e, a, b, c, 72) SHA1_R4(c, d, e, a, b, 73) SHA1_R4(b, c, d, e, a, 74) SHA1_R4(a, b, c, d, e, 75)
    SHA1_R4(e, a, b, c, d, 76) SHA1_R4(d, e, a, b, c, 77) SHA1_R4(c, d, e, a, b, 78) SHA1_R4(b, c, d, e, a, 79)

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

// VERIFY: 96d20fb3 auto
// OFFSET: 0x006CB5F0
void SHA1_Init(SHA1_CONTEXT* context) {
    context->m_state[0] = 0x67452301;
    context->m_state[1] = 0xEFCDAB89;
    context->m_state[2] = 0x98BADCFE;
    context->m_state[3] = 0x10325476;
    context->m_state[4] = 0xC3D2E1F0;
    context->m_count[0] = context->m_count[1] = 0;
}

// VERIFY: 7ea85afe auto
// OFFSET: 0x006CB630
void SHA1_Update(SHA1_CONTEXT* context, const uint8_t* data, uint32_t length) {
    uint32_t i;
    uint32_t j = (context->m_count[0] >> 3) & 63;

    if ((context->m_count[0] += length << 3) < (length << 3)) {
        context->m_count[1]++;
    }

    context->m_count[1] += length >> 29;

    if (j + length > 63) {
        i = 64 - j;
        memcpy(&context->m_buffer[j], data, i);
        SHA1_Transform(context->m_state, context->m_buffer);

        for (; i + 63 < length; i += 64) {
            SHA1_Transform(context->m_state, const_cast<uint8_t*>(&data[i]));
        }

        j = 0;
    } else {
        i = 0;
    }

    memcpy(&context->m_buffer[j], &data[i], length - i);
}

// VERIFY: 91dea06a manual
// OFFSET: 0x006CB6F0
void SHA1_Final(uint8_t* digest, SHA1_CONTEXT* context) {
    uint8_t finalcount[8];

    for (uint32_t i = 0; i < 8; i++) {
        finalcount[i] = static_cast<uint8_t>(context->m_count[i >= 4 ? 0 : 1] >> ((3 - (i & 3)) * 8));
    }

    SHA1_Update(context, reinterpret_cast<const uint8_t*>("\200"), 1);

    while ((context->m_count[0] & 504) != 448) {
        SHA1_Update(context, reinterpret_cast<const uint8_t*>("\0"), 1);
    }

    SHA1_Update(context, finalcount, 8);

    for (uint32_t i = 0; i < 20; i++) {
        digest[i] = static_cast<uint8_t>(context->m_state[i >> 2] >> ((3 - (i & 3)) * 8));
    }
}

// VERIFY: 32893a32 manual
// OFFSET: 0x006CB7C0
uint8_t* SHA1_InterleaveHash(uint8_t* out, const uint8_t* in, uint32_t length) {
    while (length && !*in) {
        in++;
        length--;
    }

    if (length & 1) {
        in++;
        length--;
    }

    uint32_t half = length >> 1;
    uint8_t* temp = static_cast<uint8_t*>(_alloca(half));

    if (!temp) {
        return nullptr;
    }

    SHA1_CONTEXT context;
    uint8_t digest[20];

    for (uint32_t i = 0; i < half; i++) {
        temp[i] = in[i * 2];
    }

    SHA1_Init(&context);
    SHA1_Update(&context, temp, half);
    SHA1_Final(digest, &context);

    for (uint32_t i = 0; i < 20; i++) {
        out[i * 2] = digest[i];
    }

    for (uint32_t i = 0; i < half; i++) {
        temp[i] = in[i * 2 + 1];
    }

    SHA1_Init(&context);
    SHA1_Update(&context, temp, half);
    SHA1_Final(digest, &context);

    for (uint32_t i = 0; i < 20; i++) {
        out[i * 2 + 1] = digest[i];
    }

    return out;
}
