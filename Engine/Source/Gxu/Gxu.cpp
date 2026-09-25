#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Tempest/CImVector.h"

// VERIFY: ad3a494d manual
// OFFSET: 0x006BFDA0
void GxuUpdateSingleColorTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    // STATIC: 0x00C7D1B0
    static CImVector s_texels[64];

    switch (command) {
        case GxTex_Lock: {
            uint32_t color = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(userArg));

            for (uint32_t i = 0; i < 64; i++) {
                reinterpret_cast<uint32_t*>(s_texels)[i] = color;
            }

            break;
        }

        case GxTex_Latch:
            texelStrideInBytes = 4 * width;
            texels = s_texels;
            break;

        default:
            break;
    }
}
