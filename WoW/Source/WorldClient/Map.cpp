#include "Map.h"
#include <cstring>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/FVBBList.h"
#include "WoW/Source/WorldClient/Liquid.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/WaterRipple.h"

// STATIC: 0x00ADF800
float CMap::s_unk_ADF800 = 0.58333333f;

// STATIC: 0x00ADFBC4
int32_t s_mapId = -1;

// STATIC: 0x00ADFBCC
FVBBList CMap::s_vbbList;

// STATIC: 0x00AEED74
TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> CMap::s_mapRenderChunkFreeList;

// STATIC: 0x00AEED80
TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, unk_0014)> CMap::s_unk_AEED80;

// STATIC: 0x00CDF7D0
float CMap::s_unk_CDF7D0[256];

// STATIC: 0x00CDFBD0
float CMap::s_unk_CDFBD0[256];

// STATIC: 0x00CDFFF8
CGxBuf* CMap::lowDetailIndexBuf;

// STATIC: 0x00CDFFFC
CGxPool* CMap::lowDetailIndexPool;

// STATIC: 0x00CE0000
CGxShader* CMap::pixelShader_TerrainSM;

// STATIC: 0x00CE0004
CGxShader* CMap::pixelShader_Terrain0_env;

// STATIC: 0x00CE0008
CGxShader* CMap::vertexShader_Terrain[128];

// STATIC: 0x00CE0208
CGxShader* CMap::pixelShader_Terrain3[96];

// STATIC: 0x00CE0388
CGxShader* CMap::pixelShader_Terrain2[32];

// STATIC: 0x00CE0408
CGxShader* CMap::pixelShader_Terrain1[32];

// STATIC: 0x00CE0488
CGxShader* CMap::pixelShader_Terrain0[3];

// STATIC: 0x00CE04A4
int32_t CMap::uniqueId;

// STATIC: 0x00CE04AC
int32_t CMap::oldSelectLightParm;

// STATIC: 0x00CE04C4
uint32_t CMap::s_queryTag;

// STATIC: 0x00CE04C8
int32_t CMap::cCount;

// STATIC: 0x00CE08D0
uint32_t CMap::unk[4096];

// STATIC: 0x00CE48D0
CMapArea* CMap::m_areaTable[4096];

// STATIC: 0x00CE88D0
SMAreaInfo CMap::m_areaInfo[4096];

// STATIC: 0x00CF08F0
int32_t CMap::bActive;

// STATIC: 0x00CF08F4
int32_t CMap::bDungeon;

// STATIC: 0x00CF08F8
int32_t CMap::s_useSSE;

// STATIC: 0x00CF4928
TSGrowableArray<uint32_t> CMap::scCollideList;

// STATIC: 0x00D253A4
int32_t CMap::freeCounts[11];

// STATIC: 0x00D253D0
int32_t CMap::counts[11];

// STATIC: 0x00D253FC
uint32_t* CMap::lightHeap;

// STATIC: 0x00D25400
uint32_t* CMap::cacheLightHeap;

// STATIC: 0x00D25404
uint32_t* CMap::mapObjGroupHeap;

// STATIC: 0x00D25408
uint32_t* CMap::mapObjHeap;

// STATIC: 0x00D2540C
uint32_t* CMap::baseObjLinkHeap;

// STATIC: 0x00D25410
uint32_t* CMap::areaHeap;

// STATIC: 0x00D25414
uint32_t* CMap::areaMedHeap;

// STATIC: 0x00D25418
uint32_t* CMap::areaLowHeap;

// STATIC: 0x00D2541C
uint32_t* CMap::chunkHeap;

// STATIC: 0x00D25420
uint32_t* CMap::doodadDefHeap;

// STATIC: 0x00D25424
uint32_t* CMap::entityHeap;

// STATIC: 0x00CF4958
static uint32_t unk_CF4958;

// STATIC: 0x00CF495C
static uint32_t unk_CF495C;

// STATIC: 0x00CF498C
static CMapFootprint unk_CF498C[144][4];

// STATIC: 0x00CFBE60
static TSGrowableArray<CMapFootprintTexture> unk_CFBE60;

// STATIC: 0x00D25428
uint32_t* CMap::mapObjDefGroupHeap;

// STATIC: 0x00D2542C
uint32_t* CMap::mapObjDefHeap;

// STATIC: 0x00D25430
uint32_t* CMap::chunkLiquidHeap;

// VERIFY: 6c4a26d2 manual
// OFFSET: 0x0079F320
void CMap::Destroy() {
    if (CMap::bActive) {
        CMap::Unload();
    }

    WaterRipple::Destroy();
    CMapObj::Destroy();
    CMapChunk::Destroy();
    CMapArea::Destroy();
    CDetailDoodad::Destroy();
    CMap::FootprintDestroy();

    for (uint32_t i = 0; i < sizeof(CMap::vertexShader_Terrain) / sizeof(CMap::vertexShader_Terrain[0]); i++) {
        if (CMap::vertexShader_Terrain[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::vertexShader_Terrain[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain0) / sizeof(CMap::pixelShader_Terrain0[0]); i++) {
        if (CMap::pixelShader_Terrain0[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain0[i]);
        }
    }

    if (CMap::pixelShader_Terrain0_env) {
        g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain0_env);
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain1) / sizeof(CMap::pixelShader_Terrain1[0]); i++) {
        if (CMap::pixelShader_Terrain1[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain1[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]); i++) {
        if (CMap::pixelShader_Terrain2[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain2[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]); i++) {
        if (CMap::pixelShader_Terrain3[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain3[i]);
        }
    }

    if (CMap::pixelShader_TerrainSM) {
        g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_TerrainSM);
    }

    CMap::pixelShader_TerrainSM = nullptr;

    CGxBuf* indexBuf = CMap::lowDetailIndexBuf;

    if (indexBuf && indexBuf != g_theGxDevicePtr->BufStream(indexBuf->m_pool->m_target, 0, 0)) {
        g_theGxDevicePtr->BufDestroy(indexBuf);
    }

    if (CMap::lowDetailIndexPool) {
        g_theGxDevicePtr->PoolDestroy(CMap::lowDetailIndexPool);
    }

    CMap::s_vbbList.Destroy();

    if (CMap::lightHeap) {
        SMemFree(CMap::lightHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::cacheLightHeap) {
        SMemFree(CMap::cacheLightHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjGroupHeap) {
        SMemFree(CMap::mapObjGroupHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjHeap) {
        SMemFree(CMap::mapObjHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::baseObjLinkHeap) {
        SMemFree(CMap::baseObjLinkHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaHeap) {
        SMemFree(CMap::areaHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaMedHeap) {
        SMemFree(CMap::areaMedHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::areaLowHeap) {
        SMemFree(CMap::areaLowHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::chunkHeap) {
        SMemFree(CMap::chunkHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::doodadDefHeap) {
        SMemFree(CMap::doodadDefHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::entityHeap) {
        SMemFree(CMap::entityHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjDefGroupHeap) {
        SMemFree(CMap::mapObjDefGroupHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::mapObjDefHeap) {
        SMemFree(CMap::mapObjDefHeap, "delete", SLOG_FUNCTION, 0);
    }

    if (CMap::chunkLiquidHeap) {
        SMemFree(CMap::chunkLiquidHeap, "delete", SLOG_FUNCTION, 0);
    }

    CMap::s_mapRenderChunkFreeList.Clear();
    CMap::s_unk_AEED80.Clear();
}

// VERIFY: 00edcf9c manual
// OFFSET: 0x0079E3C0
void CMap::InitializeLiquidTables() {
    float scale = 1.0f / (255.0f * CMap::s_unk_ADF800);

    for (uint32_t i = 0; i < sizeof(CMap::s_unk_CDFBD0) / sizeof(CMap::s_unk_CDFBD0[0]); i++) {
        float alpha = static_cast<float>(i) * CMap::s_unk_ADF800 * scale;
        CMap::s_unk_CDFBD0[i] = alpha >= 1.0f ? 1.0f : alpha;

        float depth = static_cast<float>(i) * 0.11111111f;
        CMap::s_unk_CDF7D0[i] = depth <= 4.6666665f ? depth * 0.21428572f : 1.0f;
    }

    WaterRipple::Initialize();
    Liquid::CreateProceduralTextures();
}

// VERIFY: cd6270d0 manual
// OFFSET: 0x0079E4F0
void CMap::InitializePCFShaders() {
    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]); i++) {
        if (CMap::pixelShader_Terrain2[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain2[i]);
        }
    }

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]); i++) {
        if (CMap::pixelShader_Terrain3[i]) {
            g_theGxDevicePtr->ShaderDestroy(CMap::pixelShader_Terrain3[i]);
        }
    }

    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain2, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain2_pcf" : "Terrain2", sizeof(CMap::pixelShader_Terrain2) / sizeof(CMap::pixelShader_Terrain2[0]));
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain3, GxSh_Pixel, "Shaders\\Pixel", CShaderEffect::s_usePcfFiltering ? "Terrain3_pcf" : "Terrain3", sizeof(CMap::pixelShader_Terrain3) / sizeof(CMap::pixelShader_Terrain3[0]));
}

// VERIFY: f2cf813b manual
// OFFSET: 0x0079E7C0
void CMap::Initialize() {
    CMapArea::Initialize();
    CMapChunk::Initialize();
    CMapObj::Initialize();
    CMapObjGroup::Initialize();
    CDetailDoodad::Initialize();
    CMap::FootprintInitialize();

    memset(CMap::counts, 0, sizeof(CMap::counts));
    memset(CMap::freeCounts, 0, sizeof(CMap::freeCounts));
    memset(CMap::unk, 0, sizeof(CMap::unk));
    memset(CMap::m_areaTable, 0, sizeof(CMap::m_areaTable));

    for (uint32_t i = 0; i < sizeof(CMap::m_areaInfo) / sizeof(CMap::m_areaInfo[0]); i++) {
        CMap::m_areaInfo[i].m_flags = 0;
    }

    CMap::scCollideList.SetCount(2048);

    CMap::cCount = 0;
    CMap::s_queryTag = 0;
    CMap::uniqueId = -2;
    s_mapId = -1;
    CMap::bDungeon = 0;
    CMap::bActive = 0;
    CMap::oldSelectLightParm = 0;

    CMap::InitializeLiquidTables();

    memset(CMap::vertexShader_Terrain, 0, sizeof(CMap::vertexShader_Terrain));

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain0) / sizeof(CMap::pixelShader_Terrain0[0]); i++) {
        CMap::pixelShader_Terrain0[i] = nullptr;
    }

    CMap::pixelShader_Terrain0_env = nullptr;

    for (uint32_t i = 0; i < sizeof(CMap::pixelShader_Terrain1) / sizeof(CMap::pixelShader_Terrain1[0]); i++) {
        CMap::pixelShader_Terrain1[i] = nullptr;
    }

    CMap::InitializePCFShaders();
    CMap::pixelShader_TerrainSM = nullptr;

    g_theGxDevicePtr->ShaderCreate(CMap::vertexShader_Terrain, GxSh_Vertex, "Shaders\\Vertex", "Terrain", 128);
    g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain0, GxSh_Pixel, "Shaders\\Pixel", "Terrain0", 3);
    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain0_env, GxSh_Pixel, "Shaders\\Pixel", "Terrain0_env", 1);

    switch (g_theGxDevicePtr->Caps().m_shaderTargets[GxSh_Pixel]) {
        case 1: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 6);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 6);
            break;
        }

        case 2: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 8);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[9], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_1", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[10], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[11], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_2", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[12], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[13], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_3", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[14], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_4", 1);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[15], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w_4", 1);
            break;
        }

        case 8:
        case 9:
        case 10: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 4);
            g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_Terrain1[8], GxSh_Pixel, "Shaders\\Pixel", "Terrain1w", 4);
            break;
        }

        default: {
            g_theGxDevicePtr->ShaderCreate(CMap::pixelShader_Terrain1, GxSh_Pixel, "Shaders\\Pixel", "Terrain1", 32);
            break;
        }
    }

    g_theGxDevicePtr->ShaderCreate(&CMap::pixelShader_TerrainSM, GxSh_Pixel, "Shaders\\Pixel", "TerrainSM", 1);

    CMap::lowDetailIndexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Dynamic, 0x1800, static_cast<EGxPoolHintBits>(0), "CMap::lowDetailIndexPool");
    CMap::lowDetailIndexBuf = g_theGxDevicePtr->BufCreate(CMap::lowDetailIndexPool, sizeof(uint16_t), 0xC00, 0);
    CMap::s_vbbList.Initialize(GxPoolTarget_Vertex, GxPoolUsage_Dynamic, 16, 0x221, 24);

    CMap::lightHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapLight), 128, "WLIGHT", true));
    CMap::cacheLightHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapCacheLight), 256, "WCACHELIGHT", true));
    CMap::mapObjGroupHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjGroup), 128, "WMAPOBJGROUP", true));
    CMap::mapObjHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObj), 32, "WMAPOBJ", true));
    CMap::baseObjLinkHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapBaseObjLink), 10000, "WBASEOBJLINK", true));
    CMap::areaHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapArea), 16, "WAREA", true));
    CMap::areaMedHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapAreaMed), 16, "WAREAMED", true));
    CMap::areaLowHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapAreaLow), 16, "WAREALOW", true));
    CMap::chunkHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapChunk), 256, "WCHUNK", true));
    CMap::doodadDefHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapDoodadDef), 5000, "WDOODADDEF", true));
    CMap::entityHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapEntity), 128, "WENTITY", true));
    CMap::mapObjDefGroupHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjDefGroup), 128, "WMAPOBJDEFGROUP", true));
    CMap::mapObjDefHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjDef), 64, "WMAPOBJDEF", true));
    CMap::chunkLiquidHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CChunkLiquid), 64, "WCHUNKLIQUID", true));

    int32_t vendor;

    if (OsGetProcessorFeaturesEx(vendor) & 0x4) {
        CMap::s_useSSE = 1;
    }
}

// INLINED
CMapFootprintTexture::CMapFootprintTexture() {
    this->m_texture = nullptr;
    this->unk_0014 = 0;
}

// VERIFY: 0a85f193 manual
// OFFSET: 0x0079F860
void CMap::FootprintDestroy() {
    for (uint32_t i = 0; i < 144; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            unk_CF498C[i][j].m_link.Unlink();
        }
    }

    for (uint32_t i = 0; i < unk_CFBE60.Count(); i++) {
        if (unk_CFBE60[i].m_texture) {
            HandleClose(reinterpret_cast<HOBJECT>(unk_CFBE60[i].m_texture));
        }
    }
}

// VERIFY: f932b981 manual
// OFFSET: 0x007A03C0
void CMap::FootprintInitialize() {
    CStatus status;

    unk_CF495C = 64;
    unk_CF4958 = 0;
    unk_CFBE60.SetCount(g_footprintTexturesDB.m_maxID + 1);

    int32_t index = g_footprintTexturesDB.m_numRecords;

    while (index) {
        index--;

        FootprintTexturesRec* record = (index < 0 || index >= g_footprintTexturesDB.m_numRecords) ? nullptr : &g_footprintTexturesDB.m_records[index];
        CMapFootprintTexture* footprint = &unk_CFBE60[record->m_id];

        if (!footprint->m_texture) {
            footprint->m_texture = TextureCreate(record->m_footstepFilename, CGxTexFlags(), &status, 1);
        }
    }
}
