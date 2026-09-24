#ifndef ENGINE_SOURCE_STORM_SOURCE_MD5_H
#define ENGINE_SOURCE_STORM_SOURCE_MD5_H

#include <cstdint>

class MD5 {
    public:
    /* 0000 */ uint32_t val[4];

    MD5(char* str);
    const MD5& operator=(const char* str);
    uint8_t* ToArray(uint8_t* array) const;
};

#endif
