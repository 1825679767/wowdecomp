#include "DetailDoodad.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include <cstring>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/CDetailDoodadData.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D1C4C4
static uint32_t unk_D1C4C4;

// STATIC: 0x00D1C4C8
static uint32_t unk_D1C4C8;

// STATIC: 0x00D1C4DC
static uint32_t unk_D1C4DC;

// STATIC: 0x00D1C508
static uint32_t unk_D1C508;

// STATIC: 0x00D1C488
CGxShader* CDetailDoodad::s_pixelShader[8];

// STATIC: 0x00D1C4A8
CGxShader* CDetailDoodad::s_vertexShader[6];

// STATIC: 0x00D1C4C0
int32_t CDetailDoodad::s_bPoolsDirty;

// STATIC: 0x00D1C4CC
uint32_t CDetailDoodad::s_idxBufItems;

// STATIC: 0x00D1C4D0
uint32_t CDetailDoodad::s_vtxBufItems;

// STATIC: 0x00D1C4D4
CGxPool* CDetailDoodad::s_gxIndexPool;

// STATIC: 0x00D1C4D8
CGxPool* CDetailDoodad::s_gxVertexPool;

// STATIC: 0x00D1C4E0
CGxTex* CDetailDoodad::s_alphaRampTexture;

// STATIC: 0x00D1C4E4
uint32_t* CDetailDoodad::s_instHeap;

// STATIC: 0x00D1C4F0
int32_t CDetailDoodad::s_bUseShaders;

// STATIC: 0x00D1C4F4
TSGrowableArray<CDetailDoodadData*> CDetailDoodad::s_doodadList;

// VERIFY: f3428d3d manual
// OFFSET: 0x007B11B0
void CDetailDoodad::CreateAlphaRampTexture(const void*& texels) {
    static CImVector s_rampTexels[8][64];

    CImVector color;
    color.b = 255;
    color.g = 255;
    color.r = 255;

    for (uint32_t i = 0; i < 64; i++) {
        color.a = i * 4;

        for (uint32_t row = 0; row < 8; row++) {
            s_rampTexels[row][63 - i] = color;
        }
    }

    texels = s_rampTexels;
}

// VERIFY: e5f01c86 manual
// OFFSET: 0x007B1270
void CDetailDoodad::UpdateAlphaRampTexture(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    switch (command) {
        case GxTex_Latch:
            CDetailDoodad::CreateAlphaRampTexture(texels);
            texelStrideInBytes = width * 4;
            break;

        default:
            break;
    }
}

// VERIFY: 666f7436 manual
// OFFSET: 0x007B2760
void CDetailDoodad::Initialize() {
    CDetailDoodad::s_instHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CDetailDoodadInst), 16, "WDETAILDOODADINST", true));

    uint32_t count = g_groundEffectDoodadDB.m_maxID + 1;
    CDetailDoodad::s_doodadList.SetCount(count);

    for (uint32_t i = 0; i < CDetailDoodad::s_doodadList.Count(); i++) {
        CDetailDoodad::s_doodadList[i] = nullptr;
    }

    for (uint32_t i = 0; i < count; i++) {
        GroundEffectDoodadRec* record = g_groundEffectDoodadDB.GetRecord(i);

        if (record) {
            CDetailDoodadData* data = new (__FILE__, __LINE__) CDetailDoodadData();
            data->m_record = record;
            CDetailDoodad::s_doodadList[i] = data;
        }
    }

    GxTexCreate(64, 8, GxTex_Argb8888, CGxTexFlags(), nullptr, CDetailDoodad::UpdateAlphaRampTexture, CDetailDoodad::s_alphaRampTexture);

    memset(CDetailDoodad::s_vertexShader, 0, sizeof(CDetailDoodad::s_vertexShader));
    memset(CDetailDoodad::s_pixelShader, 0, sizeof(CDetailDoodad::s_pixelShader));

    CDetailDoodad::s_vtxBufItems = 0;
    CDetailDoodad::s_idxBufItems = 0;
    unk_D1C4C8 = 0;
    unk_D1C4C4 = 0;
    unk_D1C508 = 0;
    unk_D1C4DC = 0;
    CDetailDoodad::s_gxVertexPool = nullptr;
    CDetailDoodad::s_gxIndexPool = nullptr;
    CDetailDoodad::s_bPoolsDirty = 1;

    CDetailDoodad::s_bUseShaders = 1;

    if (!(CWorld::enables & Enable_PixelShaders)) {
        CDetailDoodad::s_bUseShaders = 0;
    }

    if (!(CWorld::enables2 & Enable_VertexShader)) {
        CDetailDoodad::s_bUseShaders = 0;
    } else if (CDetailDoodad::s_bUseShaders) {
        g_theGxDevicePtr->ShaderCreate(CDetailDoodad::s_vertexShader, GxSh_Vertex, "Shaders\\Vertex", "DetailDoodad", sizeof(CDetailDoodad::s_vertexShader) / sizeof(CDetailDoodad::s_vertexShader[0]));
        g_theGxDevicePtr->ShaderCreate(CDetailDoodad::s_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "DetailDoodad", sizeof(CDetailDoodad::s_pixelShader) / sizeof(CDetailDoodad::s_pixelShader[0]));
    }
}
