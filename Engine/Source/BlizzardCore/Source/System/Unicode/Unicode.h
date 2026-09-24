#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_UNICODE_UNICODE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_UNICODE_UNICODE_H

#include <cstdint>

namespace Blizzard {
namespace Unicode {

int32_t ConvertUTF16to8(char* dst, uint32_t dstmaxchars, const uint16_t* src, int32_t srcmaxchars, uint32_t* dstchars, uint32_t* srcchars);

}
}

namespace System_Unicode {

extern const uint32_t s_firstByteMark[7];

}

#endif
