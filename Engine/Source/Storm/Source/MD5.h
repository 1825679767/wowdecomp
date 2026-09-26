#ifndef ENGINE_SOURCE_STORM_SOURCE_MD5_H
#define ENGINE_SOURCE_STORM_SOURCE_MD5_H

#include <cstdint>

struct MD5_CTX;

class MD5 {
    public:
    /* 0000 */ uint32_t val[4];

    MD5(char* str);
    const MD5& operator=(const char* str);
    uint8_t* ToArray(uint8_t* array) const;
};

void MD5Update(MD5_CTX* context, const uint8_t* input, uint32_t inputLen);

#endif
