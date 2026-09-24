#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_STRING_STRING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_STRING_STRING_H

#include <cstdint>

namespace Blizzard {
namespace String {

uint32_t Copy(char* dst, const char* src, uint32_t maxchars);

}
}

#endif
