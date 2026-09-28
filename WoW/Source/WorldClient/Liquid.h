#ifndef WOW_SOURCE_WORLDCLIENT_LIQUID_H
#define WOW_SOURCE_WORLDCLIENT_LIQUID_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Storm/H/stpl.h"

template <class T>
class IDatabase;
class CTexture;
class LiquidMaterialRec;
class LiquidTypeRec;

namespace Liquid {
    class CInstance;

    class CRenderer {
        public:
        /* 0000 */ TSGrowableArray<CInstance*> unk_0000[2];

        void Draw(CInstance* instance);
    };

    class CMaterialSettings {
        public:
        ~CMaterialSettings();
        void Destroy();
        void InitTexture();
    };

    class CSettingsBank {
        public:
        static TSGrowableArray<CMaterialSettings*> s_settings;

        static void Destroy();
        static void ResetTextures();
    };

    class IMaterial {
        public:
        virtual ~IMaterial();
        virtual void Release();
    };

    class CMaterialBank {
        public:
        static TSGrowableArray<IMaterial*> s_unk_D43B24;

        static void Destroy();
    };

    extern CDataAllocator s_unk_D31960;
    extern CDataAllocator s_unk_D31974;
    extern CDataAllocator s_unk_D31A00;
    extern CDataAllocator s_unk_D31A30;
    extern CDataAllocator s_unk_D43B00;
    extern CDataAllocator s_unk_D43B34;
    extern CDataAllocator s_unk_D45048;
    extern CDataAllocator s_unk_D4505C;
    extern CDataAllocator s_unk_D45070;
    extern CDataAllocator s_unk_D45084;
    extern CDataAllocator s_unk_D45098;
    extern CDataAllocator s_unk_D450AC;
    extern CDataAllocator s_unk_D450C0;
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
    void ClearClientEnvironments();
    void ClearGeomFactories();
    void ClearMaterials();
    void ClearWaveManagers();
    void CreateProceduralTextures();
    void DestroyProceduralTextures();
    void ExtWaterDepthTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
    int32_t Initialize(int32_t a1, float a2, int32_t a3, const char* a4, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB);
    void Shutdown();
    void WmoWaterTexCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
}

#endif
