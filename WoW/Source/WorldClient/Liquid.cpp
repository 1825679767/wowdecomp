#include "Liquid.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D31960
CDataAllocator Liquid::s_unk_D31960(0x48, 4);

// STATIC: 0x00D31974
CDataAllocator Liquid::s_unk_D31974(0x74, 0x10);

// STATIC: 0x00D31A00
CDataAllocator Liquid::s_unk_D31A00(0xC, 0x10);

// STATIC: 0x00D31A30
CDataAllocator Liquid::s_unk_D31A30(0x128, 0x10);

// STATIC: 0x00D43B00
CDataAllocator Liquid::s_unk_D43B00(0x43C, 0x20);

// STATIC: 0x00D43B24
TSGrowableArray<Liquid::IMaterial*> Liquid::CMaterialBank::s_unk_D43B24;

// STATIC: 0x00D43B34
CDataAllocator Liquid::s_unk_D43B34(0x6C, 0x20);

// STATIC: 0x00D45048
CDataAllocator Liquid::s_unk_D45048(4, 8);

// STATIC: 0x00D4505C
CDataAllocator Liquid::s_unk_D4505C(4, 8);

// STATIC: 0x00D45070
CDataAllocator Liquid::s_unk_D45070(4, 8);

// STATIC: 0x00D45084
CDataAllocator Liquid::s_unk_D45084(4, 8);

// STATIC: 0x00D45098
CDataAllocator Liquid::s_unk_D45098(4, 8);

// STATIC: 0x00D450AC
CDataAllocator Liquid::s_unk_D450AC(4, 8);

// STATIC: 0x00D450C0
CDataAllocator Liquid::s_unk_D450C0(4, 8);

// STATIC: 0x00B23F64
float Liquid::s_unk_B23F64 = 1.0f;

// STATIC: 0x00B23F68
int32_t Liquid::s_unk_B23F68 = 1;

// STATIC: 0x00D439E8
IDatabase<LiquidMaterialRec>* Liquid::s_liquidMaterialDB;

// STATIC: 0x00D439EC
IDatabase<LiquidTypeRec>* Liquid::s_liquidTypeDB;

// STATIC: 0x00D439F0
char Liquid::s_unk_D439F0[256];

// STATIC: 0x00D43AF0
int32_t Liquid::s_unk_D43AF0;

// STATIC: 0x00D43AF4
int32_t Liquid::s_unk_D43AF4;

// STATIC: 0x00D43AF8
int32_t Liquid::s_unk_D43AF8;

// STATIC: 0x00D43AFC
int32_t Liquid::s_unk_D43AFC;

// STATIC: 0x00D43B14
TSGrowableArray<Liquid::CMaterialSettings*> Liquid::CSettingsBank::s_settings;

// STATIC: 0x00D43B48
uint8_t Liquid::s_unk_D43B48[4];

// STATIC: 0x00D43B4C
CTexture* Liquid::s_unk_D43B4C;

// STATIC: 0x00D43B50
CTexture* Liquid::s_unk_D43B50[32];

// STATIC: 0x00D43BD0
CTexture* Liquid::s_wmoWaterTex;

// STATIC: 0x00D43BD4
CTexture* Liquid::s_oceanDepthTex;

// STATIC: 0x00D43BD8
CTexture* Liquid::s_riverDepthTex;

// VERIFY: fb13c55c auto
// OFFSET: 0x008A1770
int32_t Liquid::Initialize(int32_t a1, float a2, int32_t a3, const char* a4, IDatabase<LiquidTypeRec>* liquidTypeDB, IDatabase<LiquidMaterialRec>* liquidMaterialDB) {
    SStrCopy(Liquid::s_unk_D439F0, a4, sizeof(Liquid::s_unk_D439F0));
    Liquid::s_unk_B23F64 = a2;
    Liquid::s_unk_D43AF8++;
    Liquid::s_unk_D43AFC++;
    Liquid::s_unk_D43AF4 = a1;
    Liquid::s_unk_D43AF0 = a3;
    Liquid::s_unk_B23F68 = 1;
    Liquid::s_liquidTypeDB = liquidTypeDB;
    Liquid::s_liquidMaterialDB = liquidMaterialDB;
    return 1;
}

// VERIFY: 2a7eaf1c manual
// OFFSET: 0x008A2780
void Liquid::CSettingsBank::ResetTextures() {
    for (uint32_t i = 0; i < Liquid::CSettingsBank::s_settings.Count(); i++) {
        if (Liquid::CSettingsBank::s_settings[i]) {
            Liquid::CSettingsBank::s_settings[i]->Destroy();
            Liquid::CSettingsBank::s_settings[i]->InitTexture();
        }
    }
}

// VERIFY: bbba7190 auto
// OFFSET: 0x008A2A10
void Liquid::CacheProceduralTextures() {
    CGxTexFlags flags;

    TextureCacheNewTexture(Liquid::s_riverDepthTex);
    TextureCacheNewTexture(Liquid::s_oceanDepthTex);
    TextureCacheNewTexture(Liquid::s_wmoWaterTex);

    for (uint32_t i = 0; i < sizeof(Liquid::s_unk_D43B50) / sizeof(Liquid::s_unk_D43B50[0]); i++) {
        if (Liquid::s_unk_D43B50[i]) {
            TextureCacheNewTexture(Liquid::s_unk_D43B50[i]);
        }
    }

    if (Liquid::s_unk_D43B4C) {
        TextureCacheNewTexture(Liquid::s_unk_D43B4C);
    }

    Liquid::CSettingsBank::ResetTextures();
}

// VERIFY: 5d576297 auto
// OFFSET: 0x008A2E20
void Liquid::CreateProceduralTextures() {
    Liquid::s_unk_D43B48[3] = 0;
    Liquid::s_unk_D43B48[2] = 0;
    Liquid::s_unk_D43B48[1] = 0;
    Liquid::s_unk_D43B48[0] = 0;

    CGxTexFlags flags;
    Liquid::s_riverDepthTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, flags, reinterpret_cast<void*>(1), Liquid::ExtWaterDepthTexCallback, "proceduralRiverDepthTex", 0);

    flags = CGxTexFlags();
    Liquid::s_oceanDepthTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, flags, nullptr, Liquid::ExtWaterDepthTexCallback, "proceduralOceanDepthTex", 0);

    flags = CGxTexFlags();
    Liquid::s_wmoWaterTex = TextureCreate(8, 64, GxTex_Argb8888, GxTex_Argb8888, flags, nullptr, Liquid::WmoWaterTexCallback, "proceduralWmoWaterTex", 0);

    Liquid::CacheProceduralTextures();
}

// VERIFY: a171683e auto
// OFFSET: 0x007D6670
void Liquid::ClearWaveManagers() {
    Liquid::s_unk_D31A30.Clear(".?AVCWaveManager@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: 36347972 auto
// OFFSET: 0x007D47D0
void Liquid::ClearGeomFactories() {
    Liquid::s_unk_D31960.Clear(".?AVCMeshGeomFactory@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D31974.Clear(".?AVCChunkGeomFactory@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: e3c196d2 auto
// OFFSET: 0x007D5100
void Liquid::ClearClientEnvironments() {
    Liquid::s_unk_D31A00.Clear(".?AVCClientEnvironment@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: c3031eda manual
// OFFSET: 0x008A2400
void Liquid::Shutdown() {
    int32_t count = Liquid::s_unk_D43AFC--;
    Liquid::s_liquidTypeDB = nullptr;
    Liquid::s_liquidMaterialDB = nullptr;

    if (count == 1) {
        Liquid::CMaterialBank::Destroy();
        Liquid::s_unk_D43AF0 = 0;
    }

    Liquid::CSettingsBank::Destroy();
    Liquid::ClearMaterials();
    Liquid::s_unk_D43B34.Clear(".?AVCInstance@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: 901024cc manual
// OFFSET: 0x008A2980
void Liquid::DestroyProceduralTextures() {
    if (Liquid::s_unk_D43B4C) {
        HandleClose(reinterpret_cast<HOBJECT>(Liquid::s_unk_D43B4C));
        Liquid::s_unk_D43B4C = nullptr;
    }

    if (Liquid::s_riverDepthTex) {
        HandleClose(reinterpret_cast<HOBJECT>(Liquid::s_riverDepthTex));
        Liquid::s_riverDepthTex = nullptr;
    }

    if (Liquid::s_oceanDepthTex) {
        HandleClose(reinterpret_cast<HOBJECT>(Liquid::s_oceanDepthTex));
        Liquid::s_oceanDepthTex = nullptr;
    }

    if (Liquid::s_wmoWaterTex) {
        HandleClose(reinterpret_cast<HOBJECT>(Liquid::s_wmoWaterTex));
        Liquid::s_wmoWaterTex = nullptr;
    }

    for (uint32_t i = 0; i < sizeof(Liquid::s_unk_D43B50) / sizeof(Liquid::s_unk_D43B50[0]); i++) {
        if (Liquid::s_unk_D43B50[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(Liquid::s_unk_D43B50[i]));
            Liquid::s_unk_D43B50[i] = nullptr;
        }
    }
}

// VERIFY: 44d8beb7 manual
// OFFSET: 0x008A1F50
void Liquid::CMaterialBank::Destroy() {
    for (uint32_t i = 0; i < Liquid::CMaterialBank::s_unk_D43B24.Count(); i++) {
        if (Liquid::CMaterialBank::s_unk_D43B24[i]) {
            Liquid::CMaterialBank::s_unk_D43B24[i]->Release();
        }
    }

    Liquid::CMaterialBank::s_unk_D43B24.SetCount(0);
}

// VERIFY: f4d601a2 manual
// OFFSET: 0x008A2380
void Liquid::CSettingsBank::Destroy() {
    if (!--Liquid::s_unk_D43AF8) {
        for (uint32_t i = 0; i < Liquid::CSettingsBank::s_settings.Count(); i++) {
            if (Liquid::CSettingsBank::s_settings[i]) {
                Liquid::CSettingsBank::s_settings[i]->Destroy();

                Liquid::CMaterialSettings* settings = Liquid::CSettingsBank::s_settings[i];
                settings->~CMaterialSettings();
                Liquid::s_unk_D43B00.PutData(settings, nullptr, 0);
            }
        }

        Liquid::CSettingsBank::s_settings.SetCount(0);
    }

    Liquid::s_unk_D43B00.Clear(".?AVCMaterialSettings@Liquid@@", SLOG_OBJECT, 1);
}

// VERIFY: 502f2f3f auto
// OFFSET: 0x008A4280
void Liquid::ClearMaterials() {
    Liquid::s_unk_D45048.Clear(".?AVCMaterialProcWater@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D4505C.Clear(".?AVCMaterialProcWaterFFP@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D45070.Clear(".?AVCMaterialWater@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D45084.Clear(".?AVCMaterialWaterNoSpec@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D45098.Clear(".?AVCMaterialWaterFFP@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D450AC.Clear(".?AVCMaterialMagma@Liquid@@", SLOG_OBJECT, 1);
    Liquid::s_unk_D450C0.Clear(".?AVCMaterialMagmaFFP@Liquid@@", SLOG_OBJECT, 1);
}
