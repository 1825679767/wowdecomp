#ifndef WOW_SOURCE_WORLDCLIENT_LIQUID_H
#define WOW_SOURCE_WORLDCLIENT_LIQUID_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

template <class T>
class IDatabase;
class CTexture;
class LiquidMaterialRec;
class LiquidTypeRec;

namespace Liquid {
    class CMaterialSettings {
        public:
        void Destroy();
        void InitTexture();
    };

    class CSettingsBank {
        public:
        static TSGrowableArray<CMaterialSettings*> s_settings;

        static void ResetTextures();
    };

    extern float s_unk_B23F64;
    extern int32_t s_unk_B23F68;
    extern IDatabase<LiquidMaterialRec>* s_liquidMaterialDB;
    extern IDatabase<LiquidTypeRec>* s_liquidTypeDB;
    extern char s_unk_D439F0[256];
    extern int32_t s_unk_D43AF0;
    extern int32_t s_unk_D43AF4;
    extern int32_t s_unk_D43AF8;
    extern int32_t s_unk_D43AFC;
    extern uint8_t s_unk_D43B48[4];
    extern CTexture* s_unk_D43B4C;
    extern CTexture* s_unk_D43B50[32];
    extern CTexture* s_wmoWaterTex;
    extern CTexture* s_oceanDepthTex;
    extern CTexture* s_riverDepthTex;

    void CacheProceduralTextures();
    void CreateProceduralTextures();
    void ExtWaterDepthTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    int32_t Initialize(int32_t a1, float a2, int32_t a3, const char* a4, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB);
    void WmoWaterTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
}

#endif
