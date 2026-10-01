#include "FFXEffects.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/PassDeath.h"
#include "WoW/Source/Game/GameClient/PassNetherBlur.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/Game/GameClient/PassFogSeed.h"
#include "WoW/Source/Game/GameClient/PassPropagateFog.h"
#include "WoW/Source/Game/GameClient/PassFogCombine.h"

// STATIC: 0x00D38140
static uint32_t* s_seedTexels;

// VERIFY: 874a5f41 manual
// OFFSET: 0x007E86A0
PassDeath::PassDeath(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_useRect = 0;
    this->m_shaders[0] = nullptr;
    this->m_shaders[1] = nullptr;

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 8 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 9 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 10) {
        g_theGxDevicePtr->ShaderCreate(this->m_shaders, GxSh_Pixel, "Shaders\\Pixel", FFX::s_useRECT ? "FFXDeath_nvts_rect" : "FFXDeath_nvts_2d", 1);
        return;
    }

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(this->m_shaders, GxSh_Pixel, "Shaders\\Pixel", "FFXDeath", 2);
}

// VERIFY: 5970ea32 manual
// OFFSET: 0x007E8770
PassDeath::~PassDeath() {
    for (uint32_t i = 0; i < sizeof(this->m_shaders) / sizeof(this->m_shaders[0]); i++) {
        if (this->m_shaders[i]) {
            g_theGxDevicePtr->ShaderDestroy(this->m_shaders[i]);
        }
    }
}

// VERIFY: e2aee2fe auto
// OFFSET: 0x007E8920
PassDeathNVRC::PassDeathNVRC(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    g_theGxDevicePtr->ShaderCreate(&this->m_shader, GxSh_Pixel, "Shaders\\Pixel", "FFXDeath_nvrc", 1);
}

// VERIFY: 22ef7cce manual
// OFFSET: 0x007EA260
EffectDeath::EffectDeath() {
    this->m_cvar = CVar::Register("ffxDeath", "full screen death effect", 1, this->m_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, nullptr, false);

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 7) {
        FFX::Tex* inputs[] = { &FFX::s_full[0] };
        FFX::Pass* pass = new (__FILE__, __LINE__) PassDeathNVRC(inputs, 1, &FFX::s_screen, GxBlend_Opaque, true);
        this->m_passes.New(pass);
        return;
    }

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] <= 0) {
        return;
    }

    FFX::Tex* deathInputs[] = { &FFX::s_full[0], &FFX::s_quart[0] };
    FFX::Tex* quartInputs[] = { &FFX::s_quart[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassBox4(deathInputs, 1, &FFX::s_quart[0], GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassGauss4(quartInputs, 1, &FFX::s_quart[0], GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassDeath(deathInputs, 2, &FFX::s_screen, GxBlend_Opaque, true);
    this->m_passes.New(pass);
}

// VERIFY: be1ec9ba manual
// OFFSET: 0x007EA420
EffectDeath::~EffectDeath() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// VERIFY: c9c9b096 manual
// OFFSET: 0x007EA470
EffectNetherWorld::EffectNetherWorld() {
    this->m_cvar = CVar::Register("ffxNetherWorld", "full screen nether world effect (for invisibility)", 1, "1", FFX::CVarCallback, 1, false, nullptr, false);

    if (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] == 7 || g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel] <= 0) {
        return;
    }

    FFX::Tex* inputs[] = { &FFX::s_full[0], &FFX::s_quart[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassNetherBlur(inputs, 1, inputs[1], GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassNetherCombine(inputs, 2, &FFX::s_screen, GxBlend_Opaque, true);
    this->m_passes.New(pass);
}

// VERIFY: 7be538d2 manual
// OFFSET: 0x007E80B0
PassPropagateFog::PassPropagateFog(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, const C2Vector* params, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->unk_005C = 0.0f;
    this->m_useRect = FFX::s_useRECT != 0;
    memcpy(this->m_params, params, sizeof(this->m_params));
    g_theGxDevicePtr->ShaderCreate(&this->m_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "FFXPropagateFog", 2);
}

// VERIFY: cb293048 auto
// OFFSET: 0x007E81B0
PassFogCombine::PassFogCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->unk_004C = 0;
    this->unk_0050 = 0;
    this->unk_0054 = 0.0f;
    this->unk_0058 = 0.0f;
    this->m_vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Static, 34848, GxPoolHintBit_Unk0, nullptr);
    this->m_indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Static, 12288, GxPoolHintBit_Unk0, nullptr);
    this->m_vertexBuf = g_theGxDevicePtr->BufCreate(this->m_vertexPool, 32, 1089, 0);
    this->m_indexBuf = g_theGxDevicePtr->BufCreate(this->m_indexPool, 2, 6144, 0);
    this->m_useRect = FFX::s_useRECT != 0;
    g_theGxDevicePtr->ShaderCreate(&this->m_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "FFXFogCombine", 2);
}

// VERIFY: be808feb manual
// OFFSET: 0x007E8990
PassNetherBlur::PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_seed.SetSeed(0);
    this->m_useRect = 0;

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(&this->m_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "FFXNetherBlur", 2);
    g_theGxDevicePtr->ShaderCreate(&this->m_vertexShader, GxSh_Vertex, "Shaders\\Vertex", "FFXNetherBlur", 1);
    this->m_seed.SetSeed(0xABCDEF01);

    for (uint32_t table = 0; table < 3; table++) {
        for (uint32_t i = 0; i < 36; i++) {
            float value = CRandom::ufloat_(this->m_seed);
            this->m_noise[table][i] = fmodf(value + value - 1.0f, 1.0f);
        }
    }

    this->m_noiseTables[0] = this->m_noise[0];
    this->unk_0280 = 0.0f;
    this->m_noiseTables[1] = this->m_noise[1];
    this->m_noiseTables[2] = this->m_noise[2];
}

// VERIFY: 46ec84d3 auto
// OFFSET: 0x007E8BD0
PassNetherCombine::PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_useRect = 0;

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(&this->m_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "FFXNetherCombine", 2);
    this->unk_003C = 0.0f;
}

// VERIFY: 2a01a46e manual
// OFFSET: 0x007E8E40
static void SeedTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    static float s_scaleX = 64.0f / width;
    static float s_scaleY = 64.0f / height;

    if (command == GxTex_Lock) {
        s_seedTexels = static_cast<uint32_t*>(SMemAlloc(4 * height * width, __FILE__, __LINE__, 0));

        for (uint32_t y = 0; y < height; y++) {
            float fy = y * s_scaleY;
            uint8_t* texel = reinterpret_cast<uint8_t*>(s_seedTexels) + 4 * width * y;

            for (uint32_t x = 0; x < width; x++) {
                float noise = PerlinNoise2D(x * s_scaleX, fy, 5);
                texel[2] = 0xFF;
                texel[1] = 0xFF;
                texel[0] = 0xFF;
                texel[3] = static_cast<uint8_t>((noise + 3.0f) * 0.25f * 255.0f + 0.5f);
                texel += 4;
            }
        }
    } else if (command == GxTex_Latch) {
        texels = s_seedTexels;
        texelStrideInBytes = 4 * width;
    } else if (command == GxTex_Unlock) {
        SMemFree(s_seedTexels, __FILE__, __LINE__, 0);
        s_seedTexels = nullptr;
    }
}

// VERIFY: dc51e0d5 manual
// OFFSET: 0x007EA5F0
EffectSwirlingFog::EffectSwirlingFog() {
    this->m_fogTex.m_texture = nullptr;
    this->m_fogTex.m_texSize.x = 0;
    this->m_fogTex.m_texSize.y = 0;
    this->m_fogTex.m_size.x = 0;
    this->m_fogTex.m_size.y = 0;
    this->m_seedTex.m_texture = nullptr;
    this->m_seedTex.m_texSize.x = 0;
    this->m_seedTex.m_texSize.y = 0;
    this->m_seedTex.m_size.x = 0;
    this->m_seedTex.m_size.y = 0;

    C2iVector seedSize;
    seedSize.x = 256;
    seedSize.y = 256;
    C2iVector fogSize;
    fogSize.x = 256;
    fogSize.y = 128;
    FFX::CreateTex(this->m_seedTex, seedSize, false, false);
    FFX::CreateTex(this->m_fogTex, fogSize, false, true);
    TextureSetUserData(this->m_seedTex.m_texture, SeedTextureCallback, nullptr);

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 0;
    rect.maxX = 0;
    GxTexUpdate(TextureGetGxTex(this->m_seedTex.m_texture, 1, nullptr), rect, 1);

    this->m_cvar = CVar::Register("ffxSpecial", "full screen test effect", 1, this->m_defaultEnabled ? "1" : "0", FFX::CVarCallback, 1, false, nullptr, false);

    static C2Vector s_propagateParams[4] = {
        C2Vector(-1.0f, 1.0f),
        C2Vector(0.0f, 1.0f),
        C2Vector(1.0f, 1.0f),
        C2Vector(0.0f, 2.0f),
    };

    FFX::Tex* seedInputs[] = { &this->m_seedTex, &FFX::s_full[0] };
    FFX::Tex* fogInputs[] = { &this->m_fogTex, &FFX::s_full[0] };

    FFX::Pass* pass = new (__FILE__, __LINE__) PassFogSeed(seedInputs, 1, &this->m_fogTex, GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassPropagateFog(fogInputs, 1, &this->m_fogTex, s_propagateParams, GxBlend_Opaque, false);
    this->m_passes.New(pass);

    pass = new (__FILE__, __LINE__) PassFogCombine(fogInputs, 2, &FFX::s_screen, GxBlend_Opaque, true);
    this->m_passes.New(pass);
}
