#ifndef ENGINE_SOURCE_GXU_GXU_H
#define ENGINE_SOURCE_GXU_GXU_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxTex.h"

void GxuUpdateSingleColorTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);

#endif
