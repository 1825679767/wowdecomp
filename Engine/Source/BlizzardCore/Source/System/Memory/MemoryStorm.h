#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_MEMORYSTORM_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_MEMORYSTORM_H

#include <cstdint>

namespace Blizzard {
namespace Memory {

void* Allocate(uint32_t bytes);
void Free(void* ptr);
void Free(void* ptr, uint32_t flags, const char* fileName, int32_t lineNumber);

}
}

#endif
