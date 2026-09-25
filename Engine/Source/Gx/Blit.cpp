#include <cstdint>
#include <cstring>
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00C60930
static BLIT_FUNCTION s_blits[BlitFormats_Last][BlitFormats_Last][BlitAlphas_Last];

// VERIFY: d2f6b1f4 manual
// OFFSET: 0x006ABC20
static void ConvertTexels3216(const uint8_t* in, void* out, const C2iVector& size, uint32_t inStride, uint32_t outStride, uint8_t aShr, uint8_t rShr, uint8_t gShr, uint8_t bShr, uint8_t aShl, uint8_t rShl, uint8_t gShl, uint8_t bShl) {
    if (size.x < 2) {
        uint16_t* out16 = static_cast<uint16_t*>(out);

        for (int32_t y = size.y; y; y--) {
            for (int32_t x = 0; x < size.x; x++) {
                const uint8_t* texel = &in[x * 4];
                out16[x] = ((texel[0] >> bShr) << bShl) | ((texel[1] >> gShr) << gShl) | ((texel[3] >> aShr) << aShl) | ((texel[2] >> rShr) << rShl);
            }

            in += inStride;
            out16 = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(out16) + outStride);
        }
    } else {
        uint32_t* out32 = static_cast<uint32_t*>(out);

        for (int32_t y = size.y; y; y--) {
            for (int32_t x = 0; x < size.x; x += 2) {
                const uint8_t* texel = &in[x * 4];
                out32[x / 2] = ((texel[0] >> bShr) << bShl) | ((texel[1] >> gShr) << gShl) | ((texel[2] >> rShr) << rShl) | ((texel[3] >> aShr) << aShl) | ((texel[4] >> bShr) << (bShl + 16)) | ((texel[5] >> gShr) << (gShl + 16)) | ((texel[7] >> aShr) << (aShl + 16)) | ((texel[6] >> rShr) << (rShl + 16));
            }

            in += inStride;
            out32 = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(out32) + outStride);
        }
    }
}

// VERIFY: 63fb8f6e auto
// OFFSET: 0x006ABE00
static void Blit_Argb8888_Argb4444(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    ConvertTexels3216(static_cast<const uint8_t*>(in), out, size, inStride, outStride, 4, 4, 4, 4, 12, 8, 4, 0);
}

// VERIFY: efe01b4f auto
// OFFSET: 0x006ABE30
static void Blit_Argb8888_Argb1555(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    ConvertTexels3216(static_cast<const uint8_t*>(in), out, size, inStride, outStride, 7, 3, 3, 3, 15, 10, 5, 0);
}

// VERIFY: 34d329d7 auto
// OFFSET: 0x006ABE60
static void Blit_Argb8888_Rgb565(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    ConvertTexels3216(static_cast<const uint8_t*>(in), out, size, inStride, outStride, 8, 3, 2, 3, 0, 11, 5, 0);
}

// VERIFY: 09a5ad9f manual
// OFFSET: 0x006ABE90
static void Blit_Argb4444_Abgr8888(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        for (int32_t x = 0; x < size.x; x++) {
            uint32_t texel = reinterpret_cast<const uint16_t*>(src)[x];
            uint8_t a = texel >> 12;
            uint8_t r = (texel >> 8) & 0xF;
            uint8_t g = (texel >> 4) & 0xF;
            uint8_t b = texel & 0xF;
            dst[x * 4 + 3] = (a << 4) | a;
            dst[x * 4 + 1] = (g << 4) | g;
            dst[x * 4 + 2] = (b << 4) | b;
            dst[x * 4 + 0] = (r << 4) | r;
        }

        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 1e40a650 manual
// OFFSET: 0x006ABF40
static void Blit_Argb8888_Abgr8888(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        for (int32_t x = 0; x < size.x; x++) {
            dst[x * 4 + 3] = src[x * 4 + 3];
            dst[x * 4 + 2] = src[x * 4 + 0];
            dst[x * 4 + 1] = src[x * 4 + 1];
            dst[x * 4 + 0] = src[x * 4 + 2];
        }

        src += inStride;
        dst += outStride;
    }
}

// VERIFY: a0aa5e63 auto
// OFFSET: 0x006ABFC0
static void Blit_Argb8888_Argb8888_A1(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        const uint32_t* srcRow = reinterpret_cast<const uint32_t*>(src);
        uint32_t* dstRow = reinterpret_cast<uint32_t*>(dst);

        for (int32_t x = 0; x < size.x; x++) {
            if (srcRow[x] & 0xFF000000) {
                dstRow[x] ^= (dstRow[x] ^ srcRow[x]) & 0x00FFFFFF;
            }
        }

        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 66b1bee5 auto
// OFFSET: 0x006AC030
static void Blit_Dxt1_Dxt1(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    int32_t width = size.x < 4 ? 4 : size.x;
    int32_t blockRows = size.y >> 2;
    if (blockRows < 1) {
        blockRows = 1;
    }

    uint32_t rowBytes = width * 2;

    if (rowBytes == inStride && rowBytes == outStride) {
        memcpy(out, in, blockRows * width * 2);
        return;
    }

    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (; blockRows; blockRows--) {
        memcpy(dst, src, rowBytes);
        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 17c6637b auto
// OFFSET: 0x006AC0B0
static void Blit_Dxt35_Dxt35(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    int32_t width = size.x < 4 ? 4 : size.x;
    int32_t blockRows = size.y >> 2;
    if (blockRows < 1) {
        blockRows = 1;
    }

    uint32_t rowBytes = width * 4;

    if (rowBytes == inStride && rowBytes == outStride) {
        memcpy(out, in, blockRows * width * 4);
        return;
    }

    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (; blockRows; blockRows--) {
        memcpy(dst, src, rowBytes);
        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 5c2ff0f7 manual
// OFFSET: 0x006AC190
static void CopyGeneric32(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    if (size.x * 4 == inStride && size.x * 4 == outStride) {
        memcpy(out, in, size.y * size.x * 4);
        return;
    }

    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        memcpy(dst, src, size.x * 4);
        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 5837c6d7 auto
// OFFSET: 0x006AC200
static void CopyGeneric16(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    if (size.x * 2 == inStride && size.x * 2 == outStride) {
        memcpy(out, in, size.y * size.x * 2);
        return;
    }

    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        memcpy(dst, src, size.x * 2);
        src += inStride;
        dst += outStride;
    }
}

// VERIFY: 0ba5f832 auto
// OFFSET: 0x006ACCE0
static void Blit_generic32_generic32(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    CopyGeneric32(size, in, inStride, out, outStride);
}

// VERIFY: 8572017d manual
// OFFSET: 0x006ACCF0
static void Blit_Argb8888_Argb8888_A8(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    const uint8_t* src = static_cast<const uint8_t*>(in);
    uint8_t* dst = static_cast<uint8_t*>(out);

    for (int32_t y = size.y; y; y--) {
        const CImVector* srcRow = reinterpret_cast<const CImVector*>(src);
        CImVector* dstRow = reinterpret_cast<CImVector*>(dst);

        for (int32_t x = 0; x < size.x; x++) {
            if ((*reinterpret_cast<const uint32_t*>(&srcRow[x]) & 0xFF000000) && srcRow[x].a) {
                dstRow[x].InterpColor(srcRow[x].a, &srcRow[x]);
            }
        }

        src += inStride;
        dst += outStride;
    }
}

// VERIFY: de295cdd auto
// OFFSET: 0x006ACD70
static void Blit_generic16_generic16(const C2iVector& size, const void* in, uint32_t inStride, void* out, uint32_t outStride) {
    CopyGeneric16(size, in, inStride, out, outStride);
}

// VERIFY: 93653188 auto
// OFFSET: 0x006AE6E0
static void InitBlit() {
    s_blits[BlitFormat_Argb8888][BlitFormat_Abgr8888][BlitAlpha_0] = Blit_Argb8888_Abgr8888;
    s_blits[BlitFormat_Argb8888][BlitFormat_Argb8888][BlitAlpha_0] = Blit_generic32_generic32;
    s_blits[BlitFormat_Argb8888][BlitFormat_Argb8888][BlitAlpha_1] = Blit_Argb8888_Argb8888_A1;
    s_blits[BlitFormat_Argb8888][BlitFormat_Argb8888][BlitAlpha_8] = Blit_Argb8888_Argb8888_A8;
    s_blits[BlitFormat_Argb8888][BlitFormat_Argb4444][BlitAlpha_0] = Blit_Argb8888_Argb4444;
    s_blits[BlitFormat_Argb8888][BlitFormat_Argb1555][BlitAlpha_0] = Blit_Argb8888_Argb1555;
    s_blits[BlitFormat_Argb8888][BlitFormat_Rgb565][BlitAlpha_0] = Blit_Argb8888_Rgb565;
    s_blits[BlitFormat_Rgb565][BlitFormat_Rgb565][BlitAlpha_0] = Blit_generic16_generic16;
    s_blits[BlitFormat_Argb4444][BlitFormat_Abgr8888][BlitAlpha_0] = Blit_Argb4444_Abgr8888;
    s_blits[BlitFormat_Argb4444][BlitFormat_Argb4444][BlitAlpha_0] = Blit_generic16_generic16;
    s_blits[BlitFormat_Argb1555][BlitFormat_Argb1555][BlitAlpha_0] = Blit_generic16_generic16;
    s_blits[BlitFormat_Dxt1][BlitFormat_Dxt1][BlitAlpha_0] = Blit_Dxt1_Dxt1;
    s_blits[BlitFormat_Dxt3][BlitFormat_Dxt3][BlitAlpha_0] = Blit_Dxt35_Dxt35;
    s_blits[BlitFormat_Dxt5][BlitFormat_Dxt5][BlitAlpha_0] = Blit_Dxt35_Dxt35;
    s_blits[BlitFormat_Dxt1][BlitFormat_Rgb565][BlitAlpha_0] = Blit_Dxt1_Rgb565;
    s_blits[BlitFormat_Dxt1][BlitFormat_Argb1555][BlitAlpha_0] = Blit_Dxt1_Argb1555;
    s_blits[BlitFormat_Dxt1][BlitFormat_Argb8888][BlitAlpha_0] = Blit_Dxt1_Argb8888;
    s_blits[BlitFormat_Dxt3][BlitFormat_Argb4444][BlitAlpha_0] = Blit_Dxt3_Argb4444;
    s_blits[BlitFormat_Dxt3][BlitFormat_Argb8888][BlitAlpha_0] = Blit_Dxt3_Argb8888;
    s_blits[BlitFormat_Dxt5][BlitFormat_Argb4444][BlitAlpha_0] = Blit_Dxt5_Argb4444;
    s_blits[BlitFormat_Dxt5][BlitFormat_Argb8888][BlitAlpha_0] = Blit_Dxt5_Argb8888;
    s_blits[BlitFormat_Uv88][BlitFormat_Uv88][BlitAlpha_0] = Blit_generic16_generic16;
    s_blits[BlitFormat_Gr1616F][BlitFormat_Gr1616F][BlitAlpha_0] = Blit_generic32_generic32;
    s_blits[BlitFormat_R32F][BlitFormat_R32F][BlitAlpha_0] = Blit_generic32_generic32;
    s_blits[BlitFormat_D24X8][BlitFormat_D24X8][BlitAlpha_0] = Blit_generic32_generic32;
}

// VERIFY: f1f74b3d auto
// OFFSET: 0x006AE7C0
int32_t Blit(const C2iVector& size, BlitAlpha alpha, const void* in, uint32_t inStride, BlitFormat inFormat, void* out, uint32_t outStride, BlitFormat outFormat) {
    // STATIC: 0x00C613C4
    static int32_t initBlit;

    if (!initBlit) {
        InitBlit();
        initBlit = 1;
    }

    BLIT_FUNCTION blit = s_blits[inFormat][outFormat][alpha];
    if (!blit) {
        return 0;
    }

    blit(size, in, inStride, out, outStride);

    return 1;
}
