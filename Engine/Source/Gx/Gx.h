#ifndef ENGINE_SOURCE_GX_GX_H
#define ENGINE_SOURCE_GX_GX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/Types.h"

int32_t GxApiSupported(EGxApi api);
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width);
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib);
uint32_t GxVertexSize(EGxVertexBufferFormat format);

#endif
