#include <cstddef>
#include <cstdint>
#include "Storm/H/storm.h"

// STATIC: 0x00A3A240
static const uint32_t offsetsFromUTF8[6] = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080 };

// STATIC: 0x00A3A258
static const uint8_t trailingBytesForUTF8[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

// STATIC: 0x00A3A358
static const uint32_t firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

// VERIFY: ee330e95 manual
// OFFSET: 0x00775BD0
int32_t STORMAPI SUniConvertUTF16to8(char* dst, uint32_t dstmaxchars, const uint16_t* src, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars) {
    char* dstStart = dst;
    const uint16_t* srcStart = src;
    const uint16_t* srcEnd = srcmaxchars == 0x7FFFFFFF ? reinterpret_cast<const uint16_t*>(-1) : src + srcmaxchars;
    char* dstEnd = dst + dstmaxchars;
    int32_t result;

    for (;;) {
        if (src >= srcEnd) {
            result = -1;
            break;
        }

        uint32_t ch = *src;
        uint32_t srcUnits = 1;

        if (ch - 0xD800 <= 0x3FF) {
            if (src + 1 >= srcEnd) {
                result = -1;
                break;
            }

            uint32_t ch2 = src[1];

            if (ch2 - 0xDC00 <= 0x3FF) {
                ch = ((ch - 0xD800) << 10) + (ch2 - 0xDC00) + 0x10000;
                srcUnits = 2;
            }
        }

        uint32_t bytesToWrite;

        if (ch < 0x80) {
            bytesToWrite = 1;

            if (!ch) {
                if (dst < dstEnd) {
                    *dst++ = 0;
                    result = 0;
                } else {
                    result = bytesToWrite;
                }

                break;
            }
        } else if (ch < 0x800) {
            bytesToWrite = 2;
        } else if (ch < 0x10000) {
            bytesToWrite = 3;
        } else if (ch < 0x200000) {
            bytesToWrite = 4;
        } else if (ch < 0x4000000) {
            bytesToWrite = 5;
        } else if (ch <= 0x7FFFFFFF) {
            bytesToWrite = 6;
        } else {
            bytesToWrite = 2;
            ch = 0xFFFD;
        }

        if (dst + bytesToWrite > dstEnd) {
            result = bytesToWrite;
            break;
        }

        dst += bytesToWrite;

        switch (bytesToWrite) {
            case 6:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 5:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 4:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 3:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 2:
                *--dst = static_cast<char>((ch & 0x3F) | 0x80);
                ch >>= 6;
            case 1:
                *--dst = static_cast<char>(ch | firstByteMark[bytesToWrite]);
        }

        dst += bytesToWrite;
        src += srcUnits;
    }

    if (srcchars) {
        *srcchars = static_cast<uint32_t>(src - srcStart);
    }

    if (dstchars) {
        *dstchars = static_cast<uint32_t>(dst - dstStart);
    }

    return result;
}

// VERIFY: b9cb0e22 manual
// OFFSET: 0x00775D90
int32_t STORMAPI SUniConvertUTF8to16Len(const char* src, uint32_t srcmaxchars, uint32_t* srcchars) {
    if (!srcmaxchars || !src) {
        if (srcchars) {
            *srcchars = 0;
        }

        return -1;
    }

    const char* source = src;
    const char* sourceEnd = srcmaxchars == 0x7FFFFFFF ? reinterpret_cast<const char*>(-1) : source + srcmaxchars;
    int32_t result = 0;

    for (;;) {
        if (source >= sourceEnd) {
            result = -1;
            break;
        }

        uint32_t extraBytesToRead = trailingBytesForUTF8[static_cast<uint8_t>(*source)];

        if (source + extraBytesToRead >= sourceEnd) {
            result = -1 - static_cast<int32_t>(extraBytesToRead);
            break;
        }

        uint32_t ch = 0;

        switch (extraBytesToRead) {
            case 5:
                ch += *source++;
                ch <<= 6;
            case 4:
                ch += *source++;
                ch <<= 6;
            case 3:
                ch += *source++;
                ch <<= 6;
            case 2:
                ch += *source++;
                ch <<= 6;
            case 1:
                ch += *source++;
                ch <<= 6;
            case 0:
                ch += *source++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (ch > 0xFFFF) {
            if (ch <= 0x10FFFF) {
                result += 2;
            } else {
                result++;
            }
        } else {
            result++;

            if (!ch) {
                break;
            }
        }
    }

    if (srcchars) {
        *srcchars = static_cast<uint32_t>(source - src);
    }

    return result;
}

// VERIFY: 0cd46101 manual
// OFFSET: 0x00775EB0
int32_t STORMAPI SUniConvertUTF8to16(uint16_t* dst, uint32_t dstmaxchars, const char* src, uint32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars) {
    if (!src || !srcmaxchars) {
        return -1;
    }

    uint16_t* dstStart = dst;
    const uint8_t* source = reinterpret_cast<const uint8_t*>(src);
    const uint8_t* sourceEnd = srcmaxchars == 0x7FFFFFFF ? reinterpret_cast<const uint8_t*>(-1) : source + srcmaxchars;
    uint16_t* dstEnd = dst + dstmaxchars;
    int32_t result;

    for (;;) {
        if (source >= sourceEnd) {
            result = -1;
            break;
        }

        uint32_t extraBytesToRead = trailingBytesForUTF8[*source];

        if (source + extraBytesToRead >= sourceEnd) {
            result = -1 - static_cast<int32_t>(extraBytesToRead);
            break;
        }

        const uint8_t* next = source;
        uint32_t ch = 0;

        switch (extraBytesToRead) {
            case 5:
                ch += *next++;
                ch <<= 6;
            case 4:
                ch += *next++;
                ch <<= 6;
            case 3:
                ch += *next++;
                ch <<= 6;
            case 2:
                ch += *next++;
                ch <<= 6;
            case 1:
                ch += *next++;
                ch <<= 6;
            case 0:
                ch += *next++;
        }

        ch -= offsetsFromUTF8[extraBytesToRead];

        if (dst >= dstEnd) {
            result = 1;
            break;
        }

        if (ch > 0xFFFF) {
            if (ch <= 0x10FFFF) {
                if (dst + 1 >= dstEnd) {
                    result = 1;
                    break;
                }

                ch -= 0x10000;
                *dst++ = static_cast<uint16_t>((ch >> 10) + 0xD800);
                *dst++ = static_cast<uint16_t>((ch & 0x3FF) + 0xDC00);
            } else {
                *dst++ = 0xFFFD;
            }
        } else {
            *dst++ = static_cast<uint16_t>(ch);

            if (!ch) {
                result = 0;
                break;
            }
        }

        source = next;
    }

    if (srcchars) {
        *srcchars = static_cast<uint32_t>(source - reinterpret_cast<const uint8_t*>(src));
    }

    if (dstchars) {
        *dstchars = static_cast<uint32_t>(dst - dstStart);
    }

    return result;
}
