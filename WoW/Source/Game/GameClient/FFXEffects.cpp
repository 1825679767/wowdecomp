#include "FFXEffects.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FFX/PassBox4.h"
#include "Engine/Source/FFX/PassGauss4.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxBuf.h"
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
#include "WoW/Source/WorldClient/FrameTime.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// STATIC: 0x00D38140
static uint32_t* s_seedTexels;

// VERIFY: 34ad1008 auto
// OFFSET: 0x007E82A0
PassFogCombine::~PassFogCombine() {
    for (uint32_t i = 0; i < sizeof(this->m_pixelShaders) / sizeof(this->m_pixelShaders[0]); i++) {
        if (this->m_pixelShaders[i]) {
            g_theGxDevicePtr->ShaderDestroy(this->m_pixelShaders[i]);
        }
    }

    CGxBuf* vertexBuf = this->m_vertexBuf;

    if (vertexBuf && vertexBuf != g_theGxDevicePtr->BufStream(vertexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(vertexBuf);
    }

    CGxBuf* indexBuf = this->m_indexBuf;

    if (indexBuf && indexBuf != g_theGxDevicePtr->BufStream(indexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(indexBuf);
    }

    if (this->m_vertexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_vertexPool);
    }

    if (this->m_indexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_indexPool);
    }
}

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
// OFFSET: [0x007E8770, 0x007E9920]
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

// VERIFY: b3213b19 manual
// OFFSET: 0x007E9950
PassDeathNVRC::~PassDeathNVRC() {
    if (this->m_shader) {
        g_theGxDevicePtr->ShaderDestroy(this->m_shader);
    }
}

// VERIFY: 7ab3fdf0 manual
// OFFSET: 0x007E8970
bool PassDeathNVRC::Valid() {
    return this->m_shader && this->m_shader->Valid();
}

// VERIFY: 45689fc4 manual
// OFFSET: 0x007E99A0
void PassDeathNVRC::Execute() {
    FFX::SetupGx(this->m_output);
    FFX::SetupGeo_Quad(this->m_output->m_size, this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, FFX::s_gridPos, FFX::s_gridTex, this->unk_002C);

    CImVector color;
    color.b = 0xA8;
    color.g = 0x93;
    color.r = 0x53;
    color.a = 0xFF;
    GxPrimVertexPtr(4, FFX::s_gridPos, sizeof(C3Vector), nullptr, 0, &color, 0, FFX::s_gridTex, sizeof(C2Vector), nullptr, 0);
    GxPrimIndexPtr(4, FFX::s_quadIndices);

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, this->m_shader);
    GxRsSet(GxRs_BlendingMode, this->m_blend);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    g_theGxDevicePtr->Draw(&FFX::s_quadBatch, 1);

    FFX::SetupReset();
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
// OFFSET: [0x007EA420, 0x007EA920]
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

// VERIFY: 0058ee1b manual
// OFFSET: 0x007E80B0
PassPropagateFog::PassPropagateFog(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, const C2Vector* params, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->unk_005C = 0.0f;
    this->m_useRect = FFX::s_useRECT != 0;
    memcpy(this->m_params, params, sizeof(this->m_params));
    g_theGxDevicePtr->ShaderCreate(this->m_pixelShaders, GxSh_Pixel, "Shaders\\Pixel", "FFXPropagateFog", 2);
}

// VERIFY: be1f217d manual
// OFFSET: [0x007E8140, 0x007E9050]
PassPropagateFog::~PassPropagateFog() {
    for (uint32_t i = 0; i < sizeof(this->m_pixelShaders) / sizeof(this->m_pixelShaders[0]); i++) {
        if (this->m_pixelShaders[i]) {
            g_theGxDevicePtr->ShaderDestroy(this->m_pixelShaders[i]);
        }
    }
}

// VERIFY: 06969c1d auto
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
    g_theGxDevicePtr->ShaderCreate(this->m_pixelShaders, GxSh_Pixel, "Shaders\\Pixel", "FFXFogCombine", 2);
}

// VERIFY: b495395e manual
// OFFSET: 0x007E8990
PassNetherBlur::PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_seed.SetSeed(0);
    this->m_useRect = 0;

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(this->m_pixelShaders, GxSh_Pixel, "Shaders\\Pixel", "FFXNetherBlur", 2);
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

// VERIFY: 32e95e2f auto
// OFFSET: 0x007E8B30
PassNetherBlur::~PassNetherBlur() {
    for (uint32_t i = 0; i < sizeof(this->m_pixelShaders) / sizeof(this->m_pixelShaders[0]); i++) {
        if (this->m_pixelShaders[i]) {
            g_theGxDevicePtr->ShaderDestroy(this->m_pixelShaders[i]);
        }
    }

    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderDestroy(this->m_vertexShader);
    }
}

// VERIFY: 8f3d2550 auto
// OFFSET: 0x007E8B90
bool PassNetherBlur::Valid() {
    if (this->m_vertexShader && this->m_vertexShader->Valid()) {
        CGxShader* shader = this->m_pixelShaders[this->m_useRect];

        if (shader && shader->Valid()) {
            return true;
        }
    }

    return false;
}

// VERIFY: 0c5ddefb auto
// OFFSET: 0x007E8BD0
PassNetherCombine::PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk) : FFX::Pass(inputs, count, output, blend, unk) {
    this->m_useRect = 0;

    if (FFX::s_useRECT) {
        this->m_useRect = 1;
    }

    g_theGxDevicePtr->ShaderCreate(this->m_pixelShaders, GxSh_Pixel, "Shaders\\Pixel", "FFXNetherCombine", 2);
    this->unk_003C = 0.0f;
}

// VERIFY: e5b4d009 manual
// OFFSET: 0x007E8C40
PassNetherCombine::~PassNetherCombine() {
    for (uint32_t i = 0; i < sizeof(this->m_pixelShaders) / sizeof(this->m_pixelShaders[0]); i++) {
        if (this->m_pixelShaders[i]) {
            g_theGxDevicePtr->ShaderDestroy(this->m_pixelShaders[i]);
        }
    }
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

// VERIFY: 07dd5136 manual
// OFFSET: [0x007EA5A0, 0x007EA950]
EffectNetherWorld::~EffectNetherWorld() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }
}

// VERIFY: 08c57bfc manual
// OFFSET: [0x007EA850, 0x007EA980]
EffectSwirlingFog::~EffectSwirlingFog() {
    for (uint32_t i = 0; i < this->m_passes.Count(); i++) {
        if (this->m_passes[i]) {
            delete this->m_passes[i];
        }
    }

    if (this->m_fogTex.m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_fogTex.m_texture));
    }

    this->m_fogTex.m_texture = nullptr;
    this->m_fogTex.m_texSize.x = 0;
    this->m_fogTex.m_texSize.y = 0;
    this->m_fogTex.m_size.x = 0;
    this->m_fogTex.m_size.y = 0;
    this->m_fogTex.m_texelSize.x = 0.0f;
    this->m_fogTex.m_texelSize.y = 0.0f;

    if (this->m_seedTex.m_texture) {
        HandleClose(reinterpret_cast<HOBJECT>(this->m_seedTex.m_texture));
    }

    this->m_seedTex.m_texture = nullptr;
    this->m_seedTex.m_texSize.x = 0;
    this->m_seedTex.m_texSize.y = 0;
    this->m_seedTex.m_size.x = 0;
    this->m_seedTex.m_size.y = 0;
    this->m_seedTex.m_texelSize.x = 0.0f;
    this->m_seedTex.m_texelSize.y = 0.0f;
}

// STATIC: 0x00D38150
static float s_netherBlurPhase;

// VERIFY: 487db931 manual
// OFFSET: 0x007E9B10
void PassNetherBlur::Execute() {
    float scale = 8.0f;
    float elapsed = FrameTime::s_tickTimeSec;
    s_netherBlurPhase = static_cast<float>(fmod(elapsed + elapsed + s_netherBlurPhase, 6.2831854820251465));

    const C44Matrix* view = FFX::GetOldViewMtx();
    float x = (view->c0 + view->b0) * 0.0f + view->a0;
    float y = (view->c1 + view->b1) * 0.0f + view->a1;
    float z = (view->c2 + view->b2) * 0.0f + view->a2;

    if (fabs(z) > 0.000099999997f) {
        float inv = 1.0f / z;
        x *= inv;
        y *= inv;
    }

    float invLength = 1.0f / sqrt(y * y + x * x);
    x *= invLength;
    y *= invLength;

    float angle = static_cast<float>(acos(x));

    if (y < 0.0f) {
        angle = 6.2831855f - angle;
    }

    angle = static_cast<float>(cos(s_netherBlurPhase) * 0.5f + angle);

    if (this->unk_0280 > 1.0f) {
        this->unk_0280 = static_cast<float>(fmod(this->unk_0280, 1.0));
        float* oldest = this->m_noiseTables[0];
        this->m_noiseTables[0] = this->m_noiseTables[1];
        this->m_noiseTables[1] = this->m_noiseTables[2];
        this->m_noiseTables[2] = oldest;

        for (uint32_t i = 0; i < 36; i++) {
            uint32_t bits = (CRandom::uint32_(this->m_seed) & 0x7FFFFF) | 0x3F800000;
            float random = *reinterpret_cast<float*>(&bits) - 1.0f;
            this->m_noiseTables[2][i] = static_cast<float>(fmod(random + random - 1.0f, 1.0));
        }
    }

    for (uint32_t i = 0; i < 36; i++) {
        this->unk_01E4[i] = (1.0f - this->unk_0280) * this->m_noiseTables[0][i] + this->m_noiseTables[1][i] * this->unk_0280;
    }

    this->unk_0280 = elapsed * 1.5f + this->unk_0280;

    FFX::SetupGx(this->m_output);

    if (!FFX::s_useRECT) {
        FFX::SetupGeo_Grid(this->m_output->m_size, this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, FFX::s_gridPos, FFX::s_gridTex, 6, this->unk_002C);
        FFX::SetupVB_PCT(36, FFX::s_gridPos, FFX::s_gridTex, this->unk_01E4);
    }

    g_theGxDevicePtr->PrimVertexMask(0x51);
    FFX::SetupIB(6, 6);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, this->m_vertexShader);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, this->m_pixelShaders[this->m_useRect]);

    float constants[4];
    constants[0] = angle;
    constants[1] = 0.0f;
    constants[2] = 0.0f;
    constants[3] = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, constants, 1);

    if (FFX::s_useRECT) {
        scale = 4.0f;
        constants[0] = 4.0f;
        constants[1] = 4.0f;
    } else {
        constants[0] = 8.0f * this->m_inputs[0]->m_texelSize.x;
        constants[1] = this->m_inputs[0]->m_texelSize.y * 8.0f;
    }

    constants[2] = 0.0f;
    constants[3] = 0.0f;
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, constants, 1);

    float convergence = g_theGxDevicePtr->StereoGetConvergence();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 2, &convergence, 1);

    CGxTex* target = TextureGetGxTex(FFX::s_quart[1].m_texture, 1, nullptr);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, target, 0);
    GxXformSetViewport(0.0f, static_cast<float>(FFX::s_quart[1].m_size.x) / FFX::s_quart[1].m_texSize.x, 0.0f, static_cast<float>(FFX::s_quart[1].m_size.y) / FFX::s_quart[1].m_texSize.y, 0.0f, 1.0f);
    GxSceneClear(1, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr));

    if (FFX::s_useRECT) {
        FFX::SetupGeo_Grid(FFX::s_quart[1].m_size, this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, FFX::s_gridPos, FFX::s_gridTex, 6, this->unk_002C);
        FFX::SetupVB_PCT(36, FFX::s_gridPos, FFX::s_gridTex, this->unk_01E4);
    }

    g_theGxDevicePtr->Draw(&FFX::s_gridBatch, 1);

    target = TextureGetGxTex(this->m_output->m_texture, 1, nullptr);
    GxRsSet(GxRs_ScissorTest, 0);
    g_theGxDevicePtr->DeviceSetRenderTarget(GxBuffers_Color, target, 0);
    GxXformSetViewport(0.0f, static_cast<float>(this->m_output->m_size.x) / this->m_output->m_texSize.x, 0.0f, static_cast<float>(this->m_output->m_size.y) / this->m_output->m_texSize.y, 0.0f, 1.0f);
    GxSceneClear(1, CImVector(0xFF, 0xFF, 0xFF, 0xFF));

    g_theGxDevicePtr->RsSet(GxRs_Texture0, TextureGetGxTex(FFX::s_quart[1].m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(FFX::s_quart[1].m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture2, TextureGetGxTex(FFX::s_quart[1].m_texture, 1, nullptr));
    g_theGxDevicePtr->RsSet(GxRs_Texture3, TextureGetGxTex(FFX::s_quart[1].m_texture, 1, nullptr));

    if (FFX::s_useRECT) {
        constants[0] = 0.0f;
        constants[1] = 0.0f;
        constants[2] = 0.0f;
        constants[3] = 0.0f;
        constants[0] = this->m_inputs[0]->m_texelSize.x / FFX::s_quart[1].m_texelSize.x * scale;
        constants[1] = scale * (this->m_inputs[0]->m_texelSize.y / FFX::s_quart[1].m_texelSize.y);
        constants[2] = 0.0f;
        constants[3] = 0.0f;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, constants, 1);
        FFX::SetupGeo_Grid(this->m_output->m_size, FFX::s_quart[1].m_size, FFX::s_quart[1].m_texSize, FFX::s_gridPos, FFX::s_gridTex, 6, this->unk_002C);
        FFX::SetupVB_PCT(36, FFX::s_gridPos, FFX::s_gridTex, this->unk_01E4);
    }

    g_theGxDevicePtr->Draw(&FFX::s_gridBatch, 1);
    FFX::SetupReset();
}

// VERIFY: 06b2c4be manual
// OFFSET: 0x007E8180
bool PassPropagateFog::Valid() {
    CGxShader* shader = this->m_pixelShaders[this->m_useRect];
    return shader && shader->Valid();
}

// VERIFY: 67b3055e manual
// OFFSET: 0x007E9080
void PassPropagateFog::Execute() {
    FFX::SetupGx(this->m_output);
    FFX::SetupGeo_Quad(this->m_output->m_size, this->m_inputs[0]->m_size, this->m_inputs[0]->m_texSize, FFX::s_gridPos, FFX::s_gridTex, this->unk_002C);
    CImVector white;
    *reinterpret_cast<uint32_t*>(&white) = 0xFFFFFFFF;
    FFX::SetupVB_PCT4(FFX::s_gridPos, white, FFX::s_gridTex, this->m_params, this->m_inputs[0]->m_texelSize);
    g_theGxDevicePtr->PrimVertexMask(0x3D1);
    FFX::SetupIB();
    CGxTex* texture = TextureGetGxTex(this->m_inputs[0]->m_texture, 1, nullptr);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture2, texture);
    g_theGxDevicePtr->RsSet(GxRs_Texture3, texture);

    for (int32_t state = GxRs_Texture4; state <= GxRs_Texture15; state++) {
        GxRsSet(static_cast<EGxRenderState>(state), 0);
    }

    g_theGxDevicePtr->RsSet(GxRs_PixelShader, this->m_pixelShaders[this->m_useRect]);
    float constants[4];
    constants[0] = this->unk_005C;
    constants[1] = constants[0];
    constants[2] = constants[0];
    constants[3] = constants[0];
    GxShaderConstantsSet(GxSh_Pixel, 0, constants, 1);
    GxRsSet(GxRs_BlendingMode, this->m_blend);
    g_theGxDevicePtr->Draw(&FFX::s_quadBatch, 1);
    FFX::SetupReset();
}

// VERIFY: 7f8524f4 auto
// OFFSET: 0x007E9010
void EffectSwirlingFog::SetParameters(int32_t count, const int32_t* params) {
    static_cast<PassFogSeed*>(this->m_passes[0])->unk_0034 = params[0];
    static_cast<PassPropagateFog*>(this->m_passes[1])->unk_005C = params[1] * 0.0039215689f;
    PassFogCombine* combine = static_cast<PassFogCombine*>(this->m_passes[2]);
    combine->unk_0054 = params[2] * 0.0099999998f;
    combine->unk_0058 = 3.0f;
}

// VERIFY: d2fd534c auto
// OFFSET: 0x007E8E20
void EffectNetherWorld::End() {
    if (this->m_passes.Count() >= 2) {
        static_cast<PassNetherCombine*>(this->m_passes[1])->unk_003C = 0.0f;
    }
}
