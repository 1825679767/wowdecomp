#include "Liquid.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

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
