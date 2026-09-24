#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
void* Blizzard::Memory::Allocate(uint32_t bytes) {
    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// VERIFY: 9ce2e935 manual
// OFFSET: 0x0042E3B0
void Blizzard::Memory::Free(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// VERIFY: 7118b5bd manual
// OFFSET: 0x00428480
void Blizzard::Memory::Free(void* ptr, uint32_t flags, const char* fileName, int32_t lineNumber) {
    SMemFree(ptr, fileName, lineNumber, (flags & 1) ? SMEM_FLAG_ZEROMEMORY : 0);
}
