#include "WaterRipple.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/stpl.h"

// STATIC: 0x00ADF7F4
static const char* unk_ADF7F4[2] = {
    "XTextures\\splash\\splash.blp",
    "XTextures\\splash\\wake.blp"
};

// STATIC: 0x00CDF7C0
static CTexture* unk_CDF7C0[2];

// STATIC: 0x00CDFFDC
static int32_t unk_CDFFDC;

// STATIC: 0x00CDFFE0
static TSFixedArray<CWaterRipple> unk_CDFFE0;

// STATIC: 0x00CDF7C8
uint32_t WaterRipple::s_nextUnitRipple;

// STATIC: 0x00CDF7CC
uint32_t WaterRipple::s_nextPlayerRipple;

// STATIC: 0x00CDFFD4
CGxShader* WaterRipple::s_vsWaterRipples;

// STATIC: 0x00CDFFD8
CGxShader* WaterRipple::s_psWaterRipples;

// INLINED
CWaterRipple::CWaterRipple() {
    this->unk_0030 = 0;
    this->unk_0034 = 0;
    this->unk_0038 = 0;
    this->unk_003C = 0;
    this->unk_0040 = 0;
    this->unk_0044 = 0;
    this->unk_0048 = 0;
    this->unk_004C = 0;
}

// VERIFY: da76fd0b auto
// OFFSET: 0x0079E1A0
void WaterRipple::Initialize() {
    WaterRipple::s_nextPlayerRipple = 0;
    WaterRipple::s_nextUnitRipple = 32;

    CGxTexFlags flags;
    CStatus status;

    for (uint32_t i = 0; i < sizeof(unk_CDF7C0) / sizeof(unk_CDF7C0[0]); i++) {
        unk_CDF7C0[i] = TextureCreate(unk_ADF7F4[i], flags, &status, 1);
    }

    unk_CDFFE0.SetCount(128);
    unk_CDFFDC = 0;

    g_theGxDevicePtr->ShaderCreate(&WaterRipple::s_vsWaterRipples, GxSh_Vertex, "Shaders\\Vertex", "WaterRipples", 1);
    g_theGxDevicePtr->ShaderCreate(&WaterRipple::s_psWaterRipples, GxSh_Pixel, "Shaders\\Pixel", "WaterRipples", 1);

    if (WaterRipple::s_vsWaterRipples && WaterRipple::s_vsWaterRipples->Valid() && WaterRipple::s_psWaterRipples && WaterRipple::s_psWaterRipples->Valid()) {
        unk_CDFFDC = 1;
    }
}
