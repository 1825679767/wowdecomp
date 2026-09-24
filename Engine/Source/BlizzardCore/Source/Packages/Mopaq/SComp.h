#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SCOMP_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SCOMP_H

#include <cstdint>

namespace System_SComp {

bool Decompress(void* dst, uint32_t* dstSize, const void* src, uint32_t srcSize);

}

#endif
