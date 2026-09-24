#include <cstdint>
#include "Engine/Source/Gx/Gx.h"

// STATIC: 0x00A2D57C
static const uint32_t s_texelSize[GxTexFormats_Last] = { 0, 4, 4, 2, 2, 2, 2, 4, 4, 2, 4, 4, 4 };

// STATIC: 0x00A2D5B0
static const int32_t s_vertexAttribOffset[GxVertexBufferFormats_Last][GxVAs_Last] = {
    { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, 24, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, 28, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, -1, -1, 24, 32, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, -1, 28, 36, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, 16, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, 12, -1, 16, 24, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, -1, -1, 12, -1, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, -1, -1, -1, 12, 20, -1, -1, -1, -1, -1, -1 },
    { 0, 12, 16, 20, -1, -1, 32, 40, -1, -1, -1, -1, -1, -1 },
    { 0, -1, -1, 12, 24, 28, 32, 40, -1, -1, -1, -1, -1, -1 }
};

// STATIC: 0x00AD8AC0
static uint32_t s_vertexSize[GxVertexBufferFormats_Last] = { 12, 24, 28, 32, 36, 40, 44, 16, 24, 32, 20, 28, 48, 48 };

// VERIFY: cef2b995 auto
// OFFSET: 0x006811D0
int32_t GxApiSupported(EGxApi api) {
    switch (api) {
        case GxApi_OpenGl:
        case GxApi_D3d9:
        case GxApi_D3d9Ex:
            return 1;

        default:
            return 0;
    }
}

// VERIFY: 619cba02 auto
// OFFSET: 0x00681230
uint32_t GxVertexSize(EGxVertexBufferFormat format) {
    return s_vertexSize[format];
}

// VERIFY: d108cfe2 auto
// OFFSET: 0x00681240
uint32_t GxVertexAttribOffset(EGxVertexBufferFormat format, EGxVertexAttrib attrib) {
    return s_vertexAttribOffset[format][attrib];
}

// VERIFY: 930fa88b manual
// OFFSET: 0x006815F0
uint32_t GxCalcTexelStrideInBytes(EGxTexFormat format, uint32_t width) {
    if (format < GxTex_Dxt1 || format > GxTex_Dxt5) {
        return s_texelSize[format] * width;
    }

    return s_texelSize[format] * ((width + 3) & ~3u);
}
