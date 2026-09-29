#include "WaterRipple.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
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

// VERIFY: ecdc54a5 manual
// OFFSET: 0x0079D5E0
void CMap::RenderWaterRipples() {
    CWaterRipple* ripple = s_unk_ADFB58.Head();

    if (!ripple) {
        return;
    }

    uint32_t vertexCounts[2] = { 0, 0 };
    TSExplicitList<CWaterRipple, offsetof(CWaterRipple, m_bucketLink)> buckets[2];

    while (ripple) {
        CWaterRipple* next = s_unk_ADFB58.Next(ripple);

        if (ripple->m_endTime <= FrameTime::s_curTimeSec) {
            ripple->m_link.Unlink();
            ripple = next;
            continue;
        }

        ripple->m_size = ripple->m_growth * FrameTime::s_tickTimeSec + ripple->m_size;
        float tick = FrameTime::s_tickTimeSec;

        if (ripple->m_fadeIn != 0.0f) {
            float alpha = tick * ripple->m_fadeIn + ripple->m_alpha;
            ripple->m_alpha = alpha;

            if (alpha > ripple->m_maxAlpha) {
                tick = tick - (alpha - ripple->m_maxAlpha) / ripple->m_fadeIn;
                ripple->m_alpha = ripple->m_maxAlpha;
                ripple->m_fadeIn = 0.0f;
            }
        }

        if (ripple->m_fadeIn == 0.0f) {
            ripple->m_alpha = tick * ripple->m_fadeOut + ripple->m_alpha;
        }

        if (ripple->m_alpha <= 0.0f) {
            ripple->m_link.Unlink();
        } else {
            vertexCounts[ripple->m_type] += ripple->m_verts.Count();
            buckets[ripple->m_type].LinkToTail(ripple);
        }

        ripple = next;
    }

    if (!vertexCounts[0] && !vertexCounts[1]) {
        return;
    }

    float polygonOffset = CWorldParam::cvar_footstepBias->m_floatValue * 0.0009765774f;

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 3);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_PolygonOffset, polygonOffset);

    C44Matrix world;
    world.d0 = -CWorldScene::s_activeWorldView.x;
    world.d1 = -CWorldScene::s_activeWorldView.y;
    world.d2 = -CWorldScene::s_activeWorldView.z;

    if (unk_CDFFDC) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, WaterRipple::s_vsWaterRipples);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, WaterRipple::s_psWaterRipples);

        CGxDevice* device = g_theGxDevicePtr;
        C44Matrix view;
        view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];
        C44Matrix projection;
        projection = device->m_projNative;

        if (device->DeviceApi() == GxApi_OpenGl) {
            projection.c0 = projection.c0 * -1.0f;
            projection.c1 = projection.c1 * -1.0f;
            projection.c2 = projection.c2 * -1.0f;
            projection.c3 = -1.0f * projection.c3;
        }

        C44Matrix worldViewProj = world * view * projection;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &worldViewProj.a0, 4);

        float pixelConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 3, pixelConstants, 1);
    } else {
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    }

    C44Matrix texMatrix;
    C44Matrix fadeMatrix;

    for (uint32_t type = 0; type < 2; type++) {
        uint32_t count = vertexCounts[type];

        if (!count) {
            continue;
        }

        CGxTex* gxTex = TextureGetGxTex(unk_CDF7C0[type], 0, nullptr);

        if (!gxTex) {
            continue;
        }

        CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), count);
        CGxVertexPCT* vertices = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
        CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), count);
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
        uint16_t index = 0;

        for (CWaterRipple* bucketRipple = buckets[type].Head(); bucketRipple; bucketRipple = buckets[type].Next(bucketRipple)) {
            if (!bucketRipple->m_verts.Count()) {
                continue;
            }

            C44Matrix spin = C44Matrix::RotationAroundZ(bucketRipple->m_rotation);
            CAaBox box;
            box.b.x = bucketRipple->m_position.x - bucketRipple->m_size;
            box.b.y = bucketRipple->m_position.y - bucketRipple->m_size;
            box.b.z = bucketRipple->m_position.z - 1.0f;
            box.t.x = bucketRipple->m_size + bucketRipple->m_position.x;
            box.t.y = bucketRipple->m_size + bucketRipple->m_position.y;
            box.t.z = bucketRipple->m_position.z + 1.0f;
            ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, &spin, 0.5f, 1);

            CImVector color;
            color.b = 0xFF;
            color.g = 0xFF;
            color.r = 0xFF;
            color.a = static_cast<uint8_t>(static_cast<int32_t>(bucketRipple->m_alpha * 255.0f));

            if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                CImVector swapped = color;
                swapped.r = color.b;
                swapped.b = color.r;
                color = swapped;
            }

            for (uint32_t i = 0; i < bucketRipple->m_verts.Count(); i++) {
                const C3Vector& vert = bucketRipple->m_verts.m_data[i];
                vertices->p = vert;
                vertices->c = color;
                vertices->tc = C2Vector(vert * texMatrix);
                *indices++ = index++;
                vertices++;
            }
        }

        g_theGxDevicePtr->BufUnlock(indexBuf, 0);
        indexBuf->unk_001C = 1;
        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
        vertexBuf->unk_001C = 1;

        if (index) {
            GxPrimVertexPtr(vertexBuf, GxVBF_PCT);
            g_theGxDevicePtr->PrimIndexPtr(indexBuf);
            g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);

            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = index;
            batch.m_minIndex = 0;
            batch.m_maxIndex = index - 1;
            g_theGxDevicePtr->Draw(&batch, 1);
        }
    }

    g_theGxDevicePtr->RsPop();

    if (!unk_CDFFDC) {
        GxXformPop(GxXform_World);
    }
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
