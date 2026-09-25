#ifndef ENGINE_SOURCE_GX_BLIT_H
#define ENGINE_SOURCE_GX_BLIT_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2iVector.h"

enum BlitAlpha {
    BlitAlpha_0 = 0,
    BlitAlpha_1 = 1,
    BlitAlpha_8 = 2,
    BlitAlpha_3 = 3,
    BlitAlphas_Last = 4,
};

typedef void (*BLIT_FUNCTION)(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);

void Blit_Dxt1_Rgb565(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt1_Argb1555(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt1_Argb8888(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt3_Argb4444(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt3_Argb8888(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt5_Argb4444(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
void Blit_Dxt5_Argb8888(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride);
uint32_t CalcRowStride(BlitFormat format, uint32_t width, uint32_t height);
int32_t Blit(const C2iVector& size, BlitAlpha alpha, const void* in, uint32_t inStride, BlitFormat inFormat, void* out, uint32_t outStride, BlitFormat outFormat);

#endif
