#include "shadow.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CiRect.h"

// STATIC: 0x00AF3E08
int32_t s_shadowLOD = 1;

// STATIC: 0x00AF3E20
static uint32_t s_shadowTexelCount = 64 * 8;

// STATIC: 0x00AF3E24
static CImVector s_shadowTexels[64 * 8];

// STATIC: 0x00D3803C
static CTexture* s_shadowModTexture;

// STATIC: 0x00D38040
static CTexture* s_shadowAddTexture;

// STATIC: 0x00D38044
static CTexture* s_shadowBlobTexture;

// VERIFY: 489bdfc7 auto
// OFFSET: 0x007E2C80
void ShadowDestroy() {
    if (s_shadowBlobTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(s_shadowBlobTexture));
    }

    if (s_shadowModTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(s_shadowModTexture));
    }

    if (s_shadowAddTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(s_shadowAddTexture));
    }

    s_shadowBlobTexture = nullptr;
    s_shadowModTexture = nullptr;
    s_shadowAddTexture = nullptr;
}

// VERIFY: 0b8064cf manual
// OFFSET: 0x007E36E0
static void ShadowAddCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    switch (command) {
        case GxTex_Lock: {
            s_shadowTexelCount = width * height;
            break;
        }

        case GxTex_Latch: {
            if (mipLevel != 0) {
                break;
            }

            texelStrideInBytes = width * sizeof(CImVector);
            texels = s_shadowTexels;

            CImVector* row = s_shadowTexels;

            for (uint32_t y = 0; y < height; y++) {
                CImVector color;
                color.b = 0xFF;
                color.g = 0xFF;
                color.r = 0xFF;

                for (uint32_t x = 0; x < width; x++) {
                    float t = static_cast<float>(x) / static_cast<float>(width - 1) * 12.0f;
                    float alpha;

                    if (t < 2.0f) {
                        alpha = t * 0.5f;
                    } else if (t < 10.0f) {
                        alpha = 1.0f;
                    } else {
                        alpha = (12.0f - t) * 0.5f;

                        if (alpha <= 0.0f) {
                            alpha = 0.0f;
                        }
                    }

                    color.a = static_cast<uint8_t>(alpha * 255.0f);
                    row[x] = color;
                }

                row += width;
            }

            break;
        }

        default:
            break;
    }
}

// VERIFY: 9a26fdc2 manual
// OFFSET: 0x007E3820
static void ShadowModCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    switch (command) {
        case GxTex_Lock: {
            s_shadowTexelCount = width * height;
            break;
        }

        case GxTex_Latch: {
            if (mipLevel != 0) {
                break;
            }

            texelStrideInBytes = width * sizeof(CImVector);
            texels = s_shadowTexels;

            CImVector* row = s_shadowTexels;

            for (uint32_t y = 0; y < height; y++) {
                for (uint32_t x = 0; x < width; x++) {
                    float t = static_cast<float>(x) / static_cast<float>(width - 1) * 12.0f;
                    float alpha;

                    if (t < 2.0f) {
                        alpha = t * 0.5f;
                    } else if (t < 10.0f) {
                        alpha = 1.0f;
                    } else {
                        alpha = (12.0f - t) * 0.5f;

                        if (alpha <= 0.0f) {
                            alpha = 0.0f;
                        }
                    }

                    uint8_t value = static_cast<uint8_t>(CMath::fint_((1.0f - alpha) * 255.0f));

                    CImVector color;
                    color.b = value;
                    color.g = value;
                    color.r = value;
                    color.a = value == 0xFF ? 0x00 : 0xFF;
                    row[x] = color;
                }

                row += width;
            }

            break;
        }

        default:
            break;
    }
}

// VERIFY: a405a61e auto
// OFFSET: 0x007E3980
void ShadowSetLOD(int32_t lod) {
    s_shadowLOD = lod;

    CiRect rect;
    rect.maxY = 8;
    rect.maxX = 64;

    if (lod == 1) {
        TextureSetUserData(s_shadowAddTexture, ShadowAddCallback, nullptr);
        TextureSetUserData(s_shadowModTexture, ShadowModCallback, nullptr);
        GxTexUpdate(TextureGetGxTex(s_shadowAddTexture, 1, nullptr), rect, 0);
        GxTexUpdate(TextureGetGxTex(s_shadowModTexture, 1, nullptr), rect, 0);
    }
}

// VERIFY: c53c978c manual
// OFFSET: 0x007E4A40
void ShadowInit() {
    CStatus status;

    s_shadowBlobTexture = TextureCreate("Textures\\ShadowBlob.blp", CGxTexFlags(), &status, 1);
    s_shadowAddTexture = TextureCreate(64, 8, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(), nullptr, ShadowAddCallback, "ShadowAdd", 0);
    s_shadowModTexture = TextureCreate(64, 8, GxTex_Argb8888, GxTex_Argb8888, CGxTexFlags(), nullptr, ShadowModCallback, "ShadowMod", 0);
    s_cvShadowLOD = CVar::Register("shadowLOD", "Unit shadow LOD", 1, "1", ShadowLODCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvExtShadowQuality = CVar::Lookup("extShadowQuality");
}

// VERIFY: d5bef9e8 auto
// OFFSET: 0x007E2C60
CGxTex* ProjectTex2dGetAddFadeTex() {
    return TextureGetGxTex(s_shadowAddTexture, 1, nullptr);
}
