#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// VERIFY: 7b459c65 manual
// OFFSET: 0x00428670
uint32_t Blizzard::String::Copy(char* dst, const char* src, uint32_t maxchars) {
    if (!maxchars || !dst) {
        return 0;
    }

    if (!src) {
        *dst = '\0';
        return 0;
    }

    char* end = dst + maxchars - 1;
    char* p = dst;

    while (p < end) {
        char c = *src++;

        if (!c) {
            break;
        }

        *p++ = c;
    }

    *p = '\0';

    return static_cast<uint32_t>(p - dst);
}
