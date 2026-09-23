#include <cstdint>
#include <cstring>
#include "MD5.h"

// VERIFY: 6581b87a manual
// OFFSET: 0x007709F0
uint8_t* MD5::ToArray(uint8_t* array) const {
    if (array) {
        memcpy(array, this->val, sizeof(this->val));
    }

    return array;
}

// VERIFY: 45750078 manual
// OFFSET: 0x00770A20
const MD5& MD5::operator=(const char* str) {
    for (int32_t i = 0; i < 4; ++i) {
        uint32_t value = 0;

        for (int32_t j = 0; j < 8; ++j) {
            char ch = str[j];
            uint32_t digit = 0;

            if (ch >= '0' && ch <= '9') {
                digit = ch - '0';
            } else if (ch >= 'a' && ch <= 'f') {
                digit = ch - 'a' + 10;
            } else if (ch >= 'A' && ch <= 'F') {
                digit = ch - 'A' + 10;
            }

            value = (value << 4) | digit;
        }

        this->val[i] = value;
        str += 8;
    }

    return *this;
}

// VERIFY: cfe9c14c auto
// OFFSET: 0x00770C70
MD5::MD5(char* str) {
    *this = str;
}
