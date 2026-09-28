#include "WaterRipple.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "WoW/Source/WorldClient/Liquid.h"
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

// STATIC: 0x00ADFB58
static TSExplicitList<CWaterRipple, offsetof(CWaterRipple, m_link)> s_unk_ADFB58;

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
    this->unk_004C = 0;
}

// VERIFY: 7160b523 manual
// OFFSET: 0x0079C110
CWaterRipple::~CWaterRipple() {
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

// VERIFY: 5c80007f auto
// OFFSET: 0x0079E460
void WaterRipple::Destroy() {
    Liquid::DestroyProceduralTextures();

    for (uint32_t i = 0; i < sizeof(unk_CDF7C0) / sizeof(unk_CDF7C0[0]); i++) {
        HandleClose(reinterpret_cast<HOBJECT>(unk_CDF7C0[i]));
    }

    if (unk_CDFFE0.Count()) {
        unk_CDFFE0.Clear();
    }

    s_unk_ADFB58.Clear();

    if (WaterRipple::s_vsWaterRipples) {
        g_theGxDevicePtr->ShaderDestroy(WaterRipple::s_vsWaterRipples);
    }

    if (WaterRipple::s_psWaterRipples) {
        g_theGxDevicePtr->ShaderDestroy(WaterRipple::s_psWaterRipples);
    }
}
