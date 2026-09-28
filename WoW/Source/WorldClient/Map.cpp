#include "Map.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include <cstring>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/FVBBList.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/WaterRipple.h"
#include <cmath>
#include "Engine/Source/Base/RCString.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/MapLoad.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Model2/M2Light.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Gxu/GxuLight.h"

// STATIC: 0x00CF4938
TSGrowableArray<CGxVertexPC> CMap::debugVertexArray;

// STATIC: 0x00CF4948
TSGrowableArray<uint16_t> CMap::debugIndexArray;

// STATIC: 0x00CE04A8
CMapLight* CMap::s_mapLight;

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

// STATIC: 0x00AEED8C
TSList<CMapArea, TSGetExplicitLink<CMapArea> > CMap::s_unk_AEED8C = TSExplicitList<CMapArea, offsetof(CMapArea, m_lameAssLink)>();

// STATIC: 0x00AEEDA4
TSList<CMapChunk, TSGetExplicitLink<CMapChunk> > CMap::s_mapChunkList = TSExplicitList<CMapChunk, offsetof(CMapChunk, m_lameAssLink)>();

// STATIC: 0x00AEEDBC
TSList<CMapObjDefGroup, TSGetExplicitLink<CMapObjDefGroup> > CMap::mapObjDefGroupList = TSExplicitList<CMapObjDefGroup, offsetof(CMapObjDefGroup, m_lameAssLink)>();

// STATIC: 0x00AEEDC8
TSList<CMapEntity, TSGetExplicitLink<CMapEntity> > CMap::entityList = TSExplicitList<CMapEntity, offsetof(CMapEntity, m_lameAssLink)>();

// STATIC: 0x00AEEDD4
TSList<CMapLight, TSGetExplicitLink<CMapLight> > CMap::lightList = TSExplicitList<CMapLight, offsetof(CMapLight, m_lameAssLink)>();

// STATIC: 0x00ADFBEC
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::s_unk_ADFBEC = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00ADFC28
TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> CMap::s_mapRenderChunkUpdateList;

// STATIC: 0x00CE049D
bool CMap::enableSpecularTerrain;

// STATIC: 0x00CE05D0
char CMap::wdtFilename[256];

// STATIC: 0x00CE06D0
char CMap::mapName[256];

// STATIC: 0x00CE07D0
char CMap::mapPath[256];

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
uint32_t CMap::cCount;

// STATIC: 0x00CE08D0
CMapAreaMed* CMap::unk[4096];

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

// STATIC: 0x00ADFC10
TSList<CMapDoodadDef, TSGetExplicitLink<CMapDoodadDef> > CMap::doodadDefList = TSExplicitList<CMapDoodadDef, offsetof(CMapDoodadDef, m_doodadDefLink)>();

// STATIC: 0x00ADFC64
C3Vector CMap::s_subchunkCoords[5] = { C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, -4.1666665f, 0.0f), C3Vector(-4.1666665f, 0.0f, 0.0f), C3Vector(-2.0833333f, -2.0833333f, 0.0f) };

// STATIC: 0x00A3FAF0
uint32_t CMap::s_holeMask[16] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000 };

// STATIC: 0x00A3FB30
int32_t CMap::s_fanIndices[8] = { 17, 0, 0, 1, 18, 17, 1, 18 };

// STATIC: 0x00A3FB50
int32_t CMap::s_subchunkIndices[8] = { 3, 0, 0, 1, 2, 3, 1, 2 };

// STATIC: 0x00D25434
TSHashTable<CMapObjDef, HASHKEY_NONE> CMap::mapObjDefHash;

// STATIC: 0x00CE0498
int32_t CMap::enableChunkBatching;

// STATIC: 0x00ADFBB4
DepthCoordTable* CMap::s_depthCoordTables[2] = { &CMap::s_unk_CDF7D0, &CMap::s_unk_CDFBD0 };

// STATIC: 0x00AEEDB0
TSList<CChunkLiquid, TSGetExplicitLink<CChunkLiquid> > CMap::s_chunkLiquidList = TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_link)>();

// STATIC: 0x00ADFC34
TSList<CChunkLiquid, TSGetExplicitLink<CChunkLiquid> > CMap::s_mapChunkLiquidUpdateList = TSExplicitList<CChunkLiquid, offsetof(CChunkLiquid, m_updateLink)>();

// STATIC: 0x00CF0900
CMapLowDetail CMap::s_lowDetail;

// STATIC: 0x00CF08D0
SMMapHeader CMap::header;

// STATIC: 0x00CE04CC
uint32_t CMap::version;

// STATIC: 0x00ADFBC8
int32_t CMap::bPreload = 1;

// STATIC: 0x00CE0494
int32_t CMap::s_isStreamingMode;

// STATIC: 0x00CE049C
bool CMap::s_unk_CE049C;

// STATIC: 0x00CE04A0
bool CMap::enableSpecular;

// STATIC: 0x00CE04A1
bool CMap::enablePixelShaders;

// STATIC: 0x00CE04A2
bool CMap::enableVertexShaders;

// STATIC: 0x00CE049E
bool CMap::gTerrainPixelShadersValid;

// STATIC: 0x00CE049F
bool CMap::enableTerrainShaderVertex;

// STATIC: 0x00CE04A3
static HASHKEY_NONE s_mapObjDefHashKey;

// STATIC: 0x00ADFC04
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::mapObjDefLinkList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00AF16CC
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::s_unk_AF16CC = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// VERIFY: ef54a4a5 auto
// OFFSET: 0x0079B870
DepthCoordTable* CMap::GetDepthCoordTable(uint32_t liquidType) {
    if (!liquidType) {
        return nullptr;
    }

    LiquidTypeRec* typeRec = g_liquidTypeDB.GetRecord(liquidType);

    if (!typeRec) {
        return nullptr;
    }

    LiquidMaterialRec* materialRec = g_liquidMaterialDB.GetRecord(typeRec->m_materialID);

    if (!materialRec || (materialRec->m_lvf != 0 && materialRec->m_lvf != 2)) {
        return nullptr;
    }

    return CMap::s_depthCoordTables[typeRec->m_int[0]];
}

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


// VERIFY: 7ce5c762 manual
// OFFSET: 0x0079E4B0
CGxShader* CMap::GetPixelShader(bool a1, bool a2, bool a3) {
    if (!a1)
        return CMap::pixelShader_Terrain0[0];
    if (!a2)
        return CMap::pixelShader_Terrain0[2];
    if (a3)
        return CMap::pixelShader_Terrain0_env;
    return CMap::pixelShader_Terrain0[1];
}

// VERIFY: cdb37c44 manual
// OFFSET: 0x007A0820
bool CMap::QueryLiquidTerrain(const C3Vector* pos, int32_t* liquidType, float* height, int32_t checkGround) {
    float cellYf = -(pos->y - 17066.666f) * 0.24f;
    float cellXf = 0.24f * -(pos->x - 17066.666f);
    int32_t cellX = CMath::fint_(cellYf - 0.5f);
    int32_t cellY = CMath::fint_(cellXf - 0.5f);

    CMapArea* area = CMap::m_areaTable[64 * ((cellY >> 7) & 0x3F) + ((cellX >> 7) & 0x3F)];

    if (!area || area->m_asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->m_mapChunks[((cellX >> 3) & 0xF) + 16 * ((cellY >> 3) & 0xF)];

    if (!chunk) {
        return false;
    }

    C2iVector tile;
    tile.x = cellX & 7;
    tile.y = cellY & 7;

    C2Vector frac(cellYf - cellX, cellXf - cellY);

    for (CChunkLiquid* liquid = chunk->m_liquidChunkLinkList.Head(); liquid; liquid = chunk->m_liquidChunkLinkList.Next(liquid)) {
        if (liquid->TileExists(tile.x, tile.y) && liquid->GetHeight(frac, tile, *height) && *height + 0.0099999998f > pos->z) {
            if (!checkGround) {
                *liquidType = liquid->m_type;
                return true;
            }

            float ground = -10000.0f;
            CMap::GetHeightTerrain(chunk, *const_cast<C3Vector*>(pos), cellX, cellY, &ground);

            if (pos->z + 0.0099999998f > ground) {
                *liquidType = liquid->m_type;
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x007A2070
static int32_t CompareIntersectCandidates(const void* a, const void* b) {
    const MapObjIntersectData* ca = static_cast<const MapObjIntersectData*>(a);
    const MapObjIntersectData* cb = static_cast<const MapObjIntersectData*>(b);

    if (cb->t > ca->t)
        return -1;

    if (cb->t < ca->t)
        return 1;

    return 0;
}

// VERIFY: 85e5829e manual
// OFFSET: 0x007A20E0
void CMap::VectorIntersectSX(CiRect& rect) {
    int32_t x = rect.minX;

    if (x <= rect.maxX) {
        do {
            CMap::scCollideList.m_data[CMap::cCount++] = x;
            CMap::scCollideList.m_data[CMap::cCount++] = rect.minY;
            x++;
        } while (x <= rect.maxX);
    } else {
        do {
            CMap::scCollideList.m_data[CMap::cCount++] = x;
            CMap::scCollideList.m_data[CMap::cCount++] = rect.minY;
            x--;
        } while (x >= rect.maxX);
    }
}

// VERIFY: 8a929a73 manual
// OFFSET: 0x007A2180
void CMap::VectorIntersectSY(CiRect& rect) {
    int32_t y = rect.minY;

    if (y <= rect.maxY) {
        do {
            CMap::scCollideList.m_data[CMap::cCount++] = rect.minX;
            CMap::scCollideList.m_data[CMap::cCount++] = y;
            y++;
        } while (y <= rect.maxY);
    } else {
        do {
            CMap::scCollideList.m_data[CMap::cCount++] = rect.minX;
            CMap::scCollideList.m_data[CMap::cCount++] = y;
            y--;
        } while (y >= rect.maxY);
    }
}

// VERIFY: 6d1fef26 manual
// OFFSET: 0x007A2230
void CMap::VectorIntersectDX(C3Vector& start, C3Vector& end, CiRect& cells) {
    int32_t y = cells.minY;
    float slope = (end.y - start.y) / (end.x - start.x);
    int32_t x = cells.minX;
    float intercept = start.y - start.x * slope;

    int32_t next;
    int32_t step;

    if (cells.maxX <= x) {
        next = cells.minX;
        step = -1;
    } else {
        next = x + 1;
        step = 1;
    }

    float edge = next * 4.1666665f;

    CMap::scCollideList.m_data[CMap::cCount++] = x;
    CMap::scCollideList.m_data[CMap::cCount++] = y;

    while (x != cells.maxX + step) {
        if (CMap::cCount >= 0x7FC) {
            return;
        }

        int32_t edgeY = CMath::fint_((slope * edge + intercept) * 0.24f - 0.5f);

        if (edgeY != y) {
            CMap::scCollideList.m_data[CMap::cCount++] = x;
            CMap::scCollideList.m_data[CMap::cCount++] = edgeY;
        }

        x += step;
        edge = step * 4.1666665f + edge;

        CMap::scCollideList.m_data[CMap::cCount++] = x;
        CMap::scCollideList.m_data[CMap::cCount++] = edgeY;

        y = edgeY;
    }

    if (y != cells.maxY) {
        CMap::scCollideList.m_data[CMap::cCount++] = cells.maxX;
        CMap::scCollideList.m_data[CMap::cCount++] = cells.maxY;
    }
}

// VERIFY: 6264b08b manual
// OFFSET: 0x007A23E0
void CMap::VectorIntersectDY(C3Vector& start, C3Vector& end, CiRect& cells) {
    float slope = (end.y - start.y) / (end.x - start.x);
    int32_t x = cells.minX;
    int32_t y = cells.minY;
    float intercept = start.y - start.x * slope;
    float invSlope = 1.0f / slope;

    int32_t next;
    int32_t step;

    if (cells.maxY <= cells.minY) {
        next = cells.minY;
        step = -1;
    } else {
        next = y + 1;
        step = 1;
    }

    float edge = next * 4.1666665f;

    CMap::scCollideList.m_data[CMap::cCount++] = cells.minX;
    CMap::scCollideList.m_data[CMap::cCount++] = cells.minY;

    while (y != cells.maxY + step) {
        if (CMap::cCount >= 0x7FC) {
            return;
        }

        int32_t edgeX = CMath::fint_((edge - intercept) * invSlope * 0.24f - 0.5f);

        if (edgeX != x) {
            CMap::scCollideList.m_data[CMap::cCount++] = edgeX;
            CMap::scCollideList.m_data[CMap::cCount++] = y;
        }

        CMap::scCollideList.m_data[CMap::cCount++] = edgeX;
        edge = step * 4.1666665f + edge;
        y += step;
        CMap::scCollideList.m_data[CMap::cCount++] = y;

        x = edgeX;
    }

    if (x != cells.maxX) {
        CMap::scCollideList.m_data[CMap::cCount++] = cells.maxX;
        CMap::scCollideList.m_data[CMap::cCount++] = cells.maxY;
    }
}


// VERIFY: 2f69e7ee auto
// OFFSET: 0x007A39F0
bool CMap::VectorIntersectTerrain(C3Vector* start, C3Vector* end, float* distance, uint32_t flags, CMapChunk** hitChunk) {
    float startX = -(start->y - 17066.666f);
    float startY = -(start->x - 17066.666f);
    float endX = -(end->y - 17066.666f);
    float endY = -(end->x - 17066.666f);

    float dX = endX - startX;
    float dY = endY - startY;

    CiRect cells;
    cells.maxY = CMath::fint_(endY * 0.24f - 0.5f);
    cells.minX = CMath::fint_(startX * 0.24f - 0.5f);
    cells.minY = CMath::fint_(startY * 0.24f - 0.5f);
    cells.maxX = CMath::fint_(endX * 0.24f - 0.5f);

    CMap::cCount = 0;

    if (fabsf(dX) < 0.00000023841858f || cells.minX == cells.maxX) {
        CMap::VectorIntersectSY(cells);
    } else if (fabsf(dY) < 0.00000023841858f || cells.minY == cells.maxY) {
        CMap::VectorIntersectSX(cells);
    } else {
        C3Vector e(endX, endY, 0.0f);
        C3Vector s(startX, startY, 0.0f);

        if (fabsf(dY) >= fabsf(dX)) {
            CMap::VectorIntersectDY(s, e, cells);
        } else {
            CMap::VectorIntersectDX(s, e, cells);
        }
    }

    return CMap::VectorIntersectSubChunkList(start, end, distance, flags, hitChunk);
}


// VERIFY: 35b33716 manual
// OFFSET: 0x007A4270
CFacet* CMap::BuildImpassableFacets(World::FacetData* facets, C3Vector* up, C3Vector* edge, C3Vector* normal, C3Vector* origin) {
    float d = -(normal->z * origin->z + normal->x * origin->x + normal->y * origin->y);

    CFacet* facet = facets->facets.New();
    facet->plane.n = *normal;
    facet->plane.d = d;
    facet->v[0] = *origin;
    facet->v[1] = { origin->x + edge->x, origin->y + edge->y, origin->z + edge->z };
    facet->v[2] = { origin->x + edge->x + up->x, origin->y + edge->y + up->y, origin->z + edge->z + up->z };

    facet = facets->facets.New();
    facet->plane.n = *normal;
    facet->plane.d = d;
    facet->v[0] = *origin;
    facet->v[1] = { origin->x + edge->x + up->x, origin->y + edge->y + up->y, origin->z + edge->z + up->z };
    facet->v[2] = { origin->x + up->x, origin->y + up->y, origin->z + up->z };

    return facet;
}

// VERIFY: f65ff37c manual
// OFFSET: 0x007A43D0
void CMap::CreateImpassableFacets(CMapChunk* chunk, CAaBox* box, World::FacetData* facets) {
    C3Vector up = { 0.0f, 0.0f, 32000.0f };
    C3Vector origin = { 0.0f, 0.0f, 0.0f };
    C3Vector normal = { 0.0f, 0.0f, 0.0f };
    C3Vector edge = { 0.0f, 0.0f, 0.0f };

    if (chunk->m_bbox.b.y > box->b.y) {
        origin = { chunk->m_bbox.b.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z };
        normal = { 0.0f, -1.0f, 0.0f };
        edge = { chunk->m_bbox.t.x - chunk->m_bbox.b.x, 0.0f, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.t.y < box->t.y) {
        origin = { chunk->m_bbox.t.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z };
        normal = { 0.0f, 1.0f, 0.0f };
        edge = { chunk->m_bbox.b.x - chunk->m_bbox.t.x, 0.0f, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.b.x > box->b.x) {
        origin = { chunk->m_bbox.b.x, chunk->m_bbox.t.y, chunk->m_bbox.b.z };
        normal = { -1.0f, 0.0f, 0.0f };
        edge = { 0.0f, chunk->m_bbox.b.y - chunk->m_bbox.t.y, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }

    if (chunk->m_bbox.t.x < box->t.x) {
        origin = { chunk->m_bbox.t.x, chunk->m_bbox.b.y, chunk->m_bbox.b.z };
        normal = { 1.0f, 0.0f, 0.0f };
        edge = { 0.0f, chunk->m_bbox.t.y - chunk->m_bbox.b.y, 0.0f };
        CMap::BuildImpassableFacets(facets, &up, &edge, &normal, &origin);
    }
}


// VERIFY: e45b1149 manual
// OFFSET: 0x007A4AF0
void CMap::AppendMapObjFacets(CMapDoodadDef* def, CAaBox* box, World::FacetData* facets) {
    uint32_t first = facets->facets.Count();

    def->m_model->GetCollisionFacets(*box, def->m_mat, facets->facets);

    uint32_t count = facets->facets.Count();
    facets->facetIds.SetCount(count);

    for (uint32_t i = first; i < count; i++) {
        facets->facetIds.m_data[i] = def->unk_00B8;
    }
}

// VERIFY: de8ab0e3 manual
// OFFSET: 0x007AD3B0
bool CMap::GetHeightTerrain(CMapChunk* chunk, C3Vector& pos, int32_t cellX, int32_t cellY, float* outHeight) {
    int32_t col = cellX & 7;
    int32_t row = cellY & 7;

    if (chunk->m_header->m_holes & CMap::s_holeMask[4 * (row >> 1) + (col >> 1)]) {
        return false;
    }

    float originX = row * -4.1666665f;
    float originY = -4.1666665f * col;

    float dx = pos.x - chunk->m_topLeftCoords.x;
    float dy = pos.y - chunk->m_topLeftCoords.y;

    int32_t fan = 0;

    if (((originX + CMap::s_subchunkCoords[2].x) - (originX + CMap::s_subchunkCoords[0].x)) * ((originY + CMap::s_subchunkCoords[2].y) - dy) - ((originY + CMap::s_subchunkCoords[2].y) - (originY + CMap::s_subchunkCoords[0].y)) * ((originX + CMap::s_subchunkCoords[2].x) - dx) <= 0.0f) {
        fan = 1;
    }

    if (((originX + CMap::s_subchunkCoords[3].x) - (originX + CMap::s_subchunkCoords[1].x)) * ((originY + CMap::s_subchunkCoords[3].y) - dy) - ((originY + CMap::s_subchunkCoords[3].y) - (originY + CMap::s_subchunkCoords[1].y)) * ((originX + CMap::s_subchunkCoords[3].x) - dx) <= 0.0f) {
        fan += 2;
    }

    float* height = &chunk->m_height[row + col + 16 * row];

    C3Vector a(originX + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan]].x, originY + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan]].y, height[CMap::s_fanIndices[2 * fan]]);
    C3Vector b(originX + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan + 1]].x, originY + CMap::s_subchunkCoords[CMap::s_subchunkIndices[2 * fan + 1]].y, height[CMap::s_fanIndices[2 * fan + 1]]);
    C3Vector c(originX + CMap::s_subchunkCoords[4].x, originY + CMap::s_subchunkCoords[4].y, height[9]);

    if (CMap::s_useSSE) {
        float ax = a.x - c.x;
        float ay = a.y - c.y;
        float az = a.z - c.z;
        float bx = b.x - c.x;
        float by = b.y - c.y;
        float bz = b.z - c.z;

        float nx = bz * ay - az * by;
        float ny = az * bx - bz * ax;
        float nz = ax * by - ay * bx;

        float scale = 1.0f / sqrtf(ny * ny + nz * nz + 0.0f * 0.0f + nx * nx);

        *outHeight = chunk->m_topLeftCoords.z - (ny * scale * dy + nx * scale * dx - (c.z * (nz * scale) + c.y * (ny * scale) + c.x * (nx * scale))) / (nz * scale);

        return true;
    }

    C4Plane plane;
    plane.From3Pos(c, a, b);

    *outHeight = chunk->m_topLeftCoords.z - (plane.n.y * dy + plane.n.x * dx + plane.d) / plane.n.z;

    return true;
}

// VERIFY: 468f4c9b manual
// OFFSET: 0x007B4090
void CMapObjDefGroup::UpdateLights() {
    for (CMapBaseObjLink* link = this->m_doodadDefLinkList.Head(); link; link = this->m_doodadDefLinkList.Next(link)) {
        link->m_owner->m_flags = static_cast<MapObjFlag>(link->m_owner->m_flags | 0x1);
    }

    for (CMapBaseObjLink* link = this->m_entityLinkList.Head(); link; link = this->m_entityLinkList.Next(link)) {
        link->m_owner->m_flags = static_cast<MapObjFlag>(link->m_owner->m_flags | 0x1);
    }
}

// VERIFY: 9eee5402 auto
// OFFSET: 0x007B4DF0
void CMap::UpdateArea(int32_t addToScene, CMapArea* area, CiRect* chunkRect, int32_t depth) {
    if (chunkRect->minX > CWorld::gbChunkRect.maxX || chunkRect->minY > CWorld::gbChunkRect.maxY || chunkRect->maxX < CWorld::gbChunkRect.minX || chunkRect->maxY < CWorld::gbChunkRect.minY) {
        area->PurgeChunks(chunkRect);
    } else if (depth == 2) {
        area->Update(addToScene, chunkRect);
    } else {
        CiRect rect;
        rect.minY = chunkRect->minY;
        rect.minX = chunkRect->minX;
        rect.maxY = chunkRect->minY + ((chunkRect->maxY - chunkRect->minY) >> 1);
        rect.maxX = chunkRect->minX + ((chunkRect->maxX - chunkRect->minX) >> 1);
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minX = ((chunkRect->maxX - chunkRect->minX) >> 1) + chunkRect->minX + 1;
        rect.maxX = chunkRect->maxX;
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minY = ((chunkRect->maxY - chunkRect->minY) >> 1) + chunkRect->minY + 1;
        rect.maxY = chunkRect->maxY;
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);

        rect.minX = chunkRect->minX;
        rect.maxX = chunkRect->minX + ((chunkRect->maxX - chunkRect->minX) >> 1);
        CMap::UpdateArea(addToScene, area, &rect, depth + 1);
    }
}

// VERIFY: fa65766b manual
// OFFSET: 0x007B5420
void CMap::ProcessChunkLiquidUpdateList() {
    CChunkLiquid* liquid = CMap::s_mapChunkLiquidUpdateList.Head();

    while (liquid) {
        CChunkLiquid* next = CMap::s_mapChunkLiquidUpdateList.Next(liquid);
        liquid->UpdateResourcePurging();

        if (liquid->m_resPurgeTimer < 0.0f) {
            liquid->m_updateLink.Unlink();
        }

        liquid = next;
    }
}

// VERIFY: 21d0bf10 auto
// OFFSET: 0x007B5500
void CMap::ProcessRenderChunkUpdateList() {
    CMapRenderChunk* renderChunk = CMap::s_mapRenderChunkUpdateList.Head();

    while (renderChunk) {
        renderChunk->m_lastUpdateTime = FrameTime::s_tickTimeSec + renderChunk->m_lastUpdateTime;
        CMapRenderChunk* next = CMap::s_mapRenderChunkUpdateList.Next(renderChunk);

        if (renderChunk->m_lastUpdateTime > 2.0f) {
            renderChunk->FreeBuf();
        }

        if (!renderChunk->m_chunkBuf) {
            renderChunk->m_renderChunkLink.Unlink();
        }

        renderChunk = next;
    }
}

// VERIFY: a5870e86 manual
// OFFSET: 0x007B5590
void CMap::PrepareEntitys(int32_t addToScene) {
    if (!addToScene) {
        return;
    }

    for (CMapEntity* entity = CMap::entityList.Head(); entity; entity = CMap::entityList.Next(entity)) {
        if (!(entity->unk_007C & 0x4)) {
            CWorldScene::AddEntityToSortTable(entity);
        }
    }
}

// VERIFY: c3245130 manual
// OFFSET: 0x007B5630
void CMap::PrepareMapDoodadDefs() {
    int32_t streaming = SFile::IsStreamingMode();
    CMapDoodadDef* doodadDef = CMap::doodadDefList.Head();

    while (doodadDef) {
        CM2Model* model = doodadDef->m_model;
        CMapDoodadDef* next = CMap::doodadDefList.Next(doodadDef);

        if (!model || model->IsLoaded(0, 0)) {
            if (!(doodadDef->unk_007C & 0x10)) {
                doodadDef->UpdateBounds();

                if (!(doodadDef->m_flags & MAPOBJ_FLAG_INTERIOR) && CMap::QueryShadow(&doodadDef->m_position)) {
                    doodadDef->m_diffuseLightScale = 0.5f;
                }

                doodadDef->ExtendChunkBounds();
                doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags | MAPOBJ_FLAG_PREPARED | MAPOBJ_FLAG_UNPLACED);
            }

            doodadDef->m_doodadDefLink.Unlink();
        } else if (streaming && (doodadDef->unk_007C & 0x10000)) {
            doodadDef->m_model->LinkToAsyncHead(1);
            doodadDef->unk_007C &= ~0x10000u;
        }

        doodadDef = next;
    }
}

// VERIFY: 6cf7464a auto
// OFFSET: 0x007BD480
int32_t CMap::SafeOpen(const char* fileName, SFileImpl** file) {
    int32_t tries = 10;

    while (!SFileOpenFile(fileName, file)) {
        DebugLog("CMap::SafeOpen() failed %s\n");

        if (!--tries) {
            SErrDisplayAppFatal("CMap::SafeOpen() failed %s", fileName);
        }
    }

    return 1;
}

// VERIFY: 3ace6e8d manual
// OFFSET: 0x007BD4D0
int32_t CMap::SafeRead(const char* fileName, SFileImpl* file, void* buffer, uint32_t size) {
    int32_t tries = 10;

    while (!SFileReadFile(file, buffer, size, nullptr, nullptr, 0)) {
        DebugLog("CMap::SafeRead() failed %s\n");

        if (!--tries) {
            DebugLog("CMap::SafeRead() failed %s");
            return 0;
        }
    }

    return 1;
}


// VERIFY: 69b638d4 auto
// OFFSET: 0x007BD540
void CMap::LoadTex() {
    char fileName[256];
    SStrCopy(fileName, CMap::wdtFilename, STORM_MAX_STR);
    char* ext = strrchr(fileName, '.');
    SStrCopy(ext, ".tex", STORM_MAX_STR);
    TextureLoadBlob(fileName);
}

// VERIFY: 4c68f31b manual
// OFFSET: 0x007BD8A0
void CMap::ValidateShaders() {
    CMap::enableVertexShaders = CWorld::enables2 & Enable_VertexShader;
    CMap::enablePixelShaders = (CWorld::enables & Enable_PixelShaders) != 0;
    CMap::enableSpecular = (CWorld::enables & Enable_Specular) && (CWorld::enables & Enable_PixelShaders);

    bool valid = false;
    bool flag = (CMap::header.m_flags >> 2) & 1;
    CMap::gTerrainPixelShadersValid = false;
    CMap::enableSpecularTerrain = false;

    if (CWorld::enables & Enable_PixelShaders) {
        CGxShader* specularShader = CMap::GetPixelShader(flag, true, false);
        CGxShader* shader = CMap::GetPixelShader(flag, false, false);
        valid = specularShader && specularShader->Valid() && shader && shader->Valid();
        CMap::gTerrainPixelShadersValid = valid;
    }

    if (CMap::enableSpecular && valid) {
        CGxShader* specularShader = CMap::GetPixelShader(flag, true, false);
        CGxShader* shader = CMap::GetPixelShader(flag, false, false);
        CMap::enableSpecularTerrain = specularShader && specularShader->Valid() && shader && shader->Valid();
    }

    if (CMap::enableVertexShaders) {
        CMap::enableTerrainShaderVertex = CMap::gTerrainPixelShadersValid && CMap::vertexShader_Terrain[0] && CMap::vertexShader_Terrain[0]->Valid();
    }

    CMap::enableChunkBatching = 0;

    if (CMap::gTerrainPixelShadersValid) {
        CMap::enableChunkBatching = CMap::enableTerrainShaderVertex != 0;
    }
}

// VERIFY: 88a28231 auto
// OFFSET: 0x007BDD70
void CMapObjDefGroup::MarkPrepared() {
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | 0x10);
}

// VERIFY: 5b940caf manual
// OFFSET: 0x007BDE50
void CMap::CreateMapObjDefGroups(CMapObj* mapObj, CMapObjDef* mapObjDef) {
    mapObjDef->m_defGroups.SetCount(mapObj->m_groupInfoCount);

    for (int32_t i = 0; i < mapObj->m_groupInfoCount; i++) {
        CMapObjDefGroup* group = CMap::AllocMapObjDefGroup();
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(group);
        link->m_ref = mapObjDef;
        mapObjDef->m_mapObjDefGroupLinkList.LinkToTail(link);
        mapObjDef->m_defGroups[i] = group;

        CAaBox box;
        mapObj->GetGroupBounds(&group->m_sphere, i);
        group->m_sphere.c *= mapObjDef->m_mat;
        mapObj->GetGroupBounds(&box, i);
        CWorldMath::TransformAABox(mapObjDef->m_mat, box, group->m_bbox);

        group->m_groupNum = i;
        group->m_ambientColor = mapObjDef->m_argbColor;
        group->m_flags = static_cast<MapObjFlag>(0);
        group->m_flags = static_cast<MapObjFlag>(group->m_flags | ((mapObj->GetGroupFlags(i) & 0x48) ? MAPOBJ_FLAG_EXTERIOR : MAPOBJ_FLAG_INTERIOR));
    }
}

// VERIFY: 1fa4bc69 manual
// OFFSET: 0x007BF460
CMapObjDef* CMap::CreateMapObjDef(char* fileName, SMMapObjDef* mapObjectDef, C3Vector* center, bool cached) {
    CMapObjDef* mapObjDef;

    if (cached && (mapObjDef = CMap::mapObjDefHash.Ptr(mapObjectDef->m_uniqueId, s_mapObjDefHashKey)) != nullptr) {
        return mapObjDef;
    }

    mapObjDef = CMap::AllocMapObjDef();

    if (cached) {
        CMap::mapObjDefHash.Insert(mapObjDef, mapObjectDef->m_uniqueId, s_mapObjDefHashKey);
    }

    mapObjDef->m_position.x = -mapObjectDef->m_position.z;
    mapObjDef->m_position.y = -mapObjectDef->m_position.x;
    mapObjDef->m_position.z = mapObjectDef->m_position.y;
    mapObjDef->m_position.x = center->x + mapObjDef->m_position.x;
    mapObjDef->m_position.y = mapObjDef->m_position.y + center->y;
    mapObjDef->m_position.z = center->z + mapObjDef->m_position.z;

    float rotX = mapObjectDef->m_rotation.z * 0.017453292f;
    float rotY = mapObjectDef->m_rotation.x * 0.017453292f;
    float rotZ = 0.017453292f * mapObjectDef->m_rotation.y;

    mapObjDef->m_flags = static_cast<MapObjFlag>(0);
    mapObjDef->m_nameId = mapObjectDef->m_nameId;
    mapObjDef->m_doodadSet = mapObjectDef->m_doodadSet;
    mapObjDef->m_nameSet = mapObjectDef->m_nameSet;
    mapObjDef->unk_0148 = 0;
    mapObjDef->unk_014C = 0;
    mapObjDef->m_doodadSetOverrides[0] = 0;
    mapObjDef->m_doodadSetOverrides[1] = 0;
    mapObjDef->m_doodadSetOverrides[2] = 0;

    mapObjDef->m_mat.d3 = 1.0f;
    mapObjDef->m_mat.c2 = 1.0f;
    mapObjDef->m_mat.b1 = 1.0f;
    mapObjDef->m_mat.a0 = 1.0f;
    mapObjDef->m_mat.d2 = 0.0f;
    mapObjDef->m_mat.d1 = 0.0f;
    mapObjDef->m_mat.d0 = 0.0f;
    mapObjDef->m_mat.c3 = 0.0f;
    mapObjDef->m_mat.c1 = 0.0f;
    mapObjDef->m_mat.c0 = 0.0f;
    mapObjDef->m_mat.b3 = 0.0f;
    mapObjDef->m_mat.b2 = 0.0f;
    mapObjDef->m_mat.b0 = 0.0f;
    mapObjDef->m_mat.a3 = 0.0f;
    mapObjDef->m_mat.a2 = 0.0f;
    mapObjDef->m_mat.a1 = 0.0f;
    mapObjDef->m_mat.d0 = mapObjDef->m_position.x;
    mapObjDef->m_mat.d1 = mapObjDef->m_position.y;
    mapObjDef->m_mat.d2 = mapObjDef->m_position.z;
    mapObjDef->m_mat.RotateAroundZ(rotZ + 3.1415927f);
    mapObjDef->m_mat.RotateAroundY(rotY);
    mapObjDef->m_mat.RotateAroundX(rotX);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

    mapObjDef->m_bbox.b.x = -mapObjectDef->m_extents.t.z;
    mapObjDef->m_bbox.b.y = -mapObjectDef->m_extents.t.x;
    mapObjDef->m_bbox.b.z = mapObjectDef->m_extents.b.y;
    mapObjDef->m_bbox.b.x = center->x + mapObjDef->m_bbox.b.x;
    mapObjDef->m_bbox.b.y = mapObjDef->m_bbox.b.y + center->y;
    mapObjDef->m_bbox.b.z = center->z + mapObjDef->m_bbox.b.z;
    mapObjDef->m_bbox.t.x = -mapObjectDef->m_extents.b.z;
    mapObjDef->m_bbox.t.y = -mapObjectDef->m_extents.b.x;
    mapObjDef->m_bbox.t.z = mapObjectDef->m_extents.t.y;
    mapObjDef->m_bbox.t.x = center->x + mapObjDef->m_bbox.t.x;
    mapObjDef->m_bbox.t.y = mapObjDef->m_bbox.t.y + center->y;
    mapObjDef->m_bbox.t.z = mapObjDef->m_bbox.t.z + center->z;

    mapObjDef->m_sphere.c.x = (mapObjDef->m_bbox.b.x + mapObjDef->m_bbox.t.x) * 0.5f;
    mapObjDef->m_sphere.c.y = (mapObjDef->m_bbox.t.y + mapObjDef->m_bbox.b.y) * 0.5f;
    mapObjDef->m_sphere.c.z = 0.5f * (mapObjDef->m_bbox.t.z + mapObjDef->m_bbox.b.z);
    float dz = mapObjDef->m_bbox.t.z - mapObjDef->m_sphere.c.z;
    float dy = mapObjDef->m_bbox.t.y - mapObjDef->m_sphere.c.y;
    float dx = mapObjDef->m_bbox.t.x - mapObjDef->m_sphere.c.x;
    mapObjDef->m_sphere.r = sqrtf(dx * dx + dy * dy + dz * dz);

    mapObjDef->m_lightsArray.m_count = 0;
    mapObjDef->m_owner = CMapObj::Create(fileName);

    return mapObjDef;
}

// VERIFY: d875b6f9 manual
// OFFSET: 0x007BF8B0
void CMap::LoadWdt() {
    SFileImpl* file = nullptr;
    SFileOpenFile(CMap::wdtFilename, &file);

    if (!file) {
        SErrDisplayAppFatal("CMap::LoadWdt() failed %s\n", CMap::wdtFilename);
    }

    SIffChunk iffChunk;
    iffChunk.m_token = 0;
    iffChunk.m_size = 0;

    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    SFileReadFile(file, &CMap::version, sizeof(CMap::version), nullptr, nullptr, 0);
    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    SFileReadFile(file, &CMap::header, sizeof(CMap::header), nullptr, nullptr, 0);
    SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
    SFileReadFile(file, CMap::m_areaInfo, sizeof(CMap::m_areaInfo), nullptr, nullptr, 0);

    if (CMap::header.m_flags & 1) {
        char fileName[256];
        SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);
        SFileReadFile(file, fileName, iffChunk.m_size, nullptr, nullptr, 0);
        SFileReadFile(file, &iffChunk, offsetof(SIffChunk, m_data), nullptr, nullptr, 0);

        SMMapObjDef def;
        SFileReadFile(file, &def, sizeof(def), nullptr, nullptr, 0);
        def.m_uniqueId = CMap::uniqueId--;

        CMapObjDef* mapObjDef = CMap::mapObjDefHash.Ptr(def.m_uniqueId, s_mapObjDefHashKey);

        if (!mapObjDef) {
            mapObjDef = CMap::AllocMapObjDef();
            CMap::mapObjDefHash.Insert(mapObjDef, def.m_uniqueId, s_mapObjDefHashKey);

            mapObjDef->m_position.x = -def.m_position.z;
            mapObjDef->m_position.y = -def.m_position.x;
            mapObjDef->m_position.z = def.m_position.y;

            float rotX = def.m_rotation.z * 0.017453292f;
            float rotY = def.m_rotation.x * 0.017453292f;
            float rotZ = 0.017453292f * def.m_rotation.y;

            mapObjDef->m_flags = static_cast<MapObjFlag>(0);
            mapObjDef->m_nameId = def.m_nameId;
            mapObjDef->m_doodadSet = def.m_doodadSet;
            mapObjDef->m_nameSet = def.m_nameSet;
            mapObjDef->unk_0148 = 0;
            mapObjDef->unk_014C = 0;
            mapObjDef->m_doodadSetOverrides[0] = 0;
            mapObjDef->m_doodadSetOverrides[1] = 0;
            mapObjDef->m_doodadSetOverrides[2] = 0;

            mapObjDef->m_mat.d3 = 1.0f;
            mapObjDef->m_mat.c2 = 1.0f;
            mapObjDef->m_mat.b1 = 1.0f;
            mapObjDef->m_mat.a0 = 1.0f;
            mapObjDef->m_mat.d2 = 0.0f;
            mapObjDef->m_mat.d1 = 0.0f;
            mapObjDef->m_mat.d0 = 0.0f;
            mapObjDef->m_mat.c3 = 0.0f;
            mapObjDef->m_mat.c1 = 0.0f;
            mapObjDef->m_mat.c0 = 0.0f;
            mapObjDef->m_mat.b3 = 0.0f;
            mapObjDef->m_mat.b2 = 0.0f;
            mapObjDef->m_mat.b0 = 0.0f;
            mapObjDef->m_mat.a3 = 0.0f;
            mapObjDef->m_mat.a2 = 0.0f;
            mapObjDef->m_mat.a1 = 0.0f;
            mapObjDef->m_mat.d0 = mapObjDef->m_position.x;
            mapObjDef->m_mat.d1 = mapObjDef->m_position.y;
            mapObjDef->m_mat.d2 = mapObjDef->m_position.z;
            mapObjDef->m_mat.RotateAroundZ(rotZ + 3.1415927f);
            mapObjDef->m_mat.RotateAroundY(rotY);
            mapObjDef->m_mat.RotateAroundX(rotX);
            mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

            mapObjDef->m_bbox.b.x = -def.m_extents.t.z;
            mapObjDef->m_bbox.b.y = -def.m_extents.t.x;
            mapObjDef->m_bbox.b.z = def.m_extents.b.y;
            mapObjDef->m_bbox.t.x = -def.m_extents.b.z;
            mapObjDef->m_bbox.t.y = -def.m_extents.b.x;
            mapObjDef->m_bbox.t.z = def.m_extents.t.y;

            mapObjDef->m_sphere.c.x = (mapObjDef->m_bbox.b.x + mapObjDef->m_bbox.t.x) * 0.5f;
            mapObjDef->m_sphere.c.y = (mapObjDef->m_bbox.t.y + mapObjDef->m_bbox.b.y) * 0.5f;
            mapObjDef->m_sphere.c.z = 0.5f * (mapObjDef->m_bbox.t.z + mapObjDef->m_bbox.b.z);
            float dz = mapObjDef->m_bbox.t.z - mapObjDef->m_sphere.c.z;
            float dy = mapObjDef->m_bbox.t.y - mapObjDef->m_sphere.c.y;
            float dx = mapObjDef->m_bbox.t.x - mapObjDef->m_sphere.c.x;
            mapObjDef->m_sphere.r = sqrtf(dx * dx + dy * dy + dz * dz);

            mapObjDef->m_lightsArray.m_count = 0;
            mapObjDef->m_owner = CMapObj::Create(fileName);
        }

        CMapBaseObjLink* link = CMap::AllocBaseObjLink(mapObjDef);
        link->m_ref = nullptr;
        CMap::mapObjDefLinkList.LinkToTail(link);
        CMap::bDungeon = 1;
    }

    if (CMap::header.m_flags & 2) {
        CMapRenderChunk::SetVertexFormat(GxVBF_PNC);
    } else {
        CMapRenderChunk::SetVertexFormat(GxVBF_PN);
    }

    CMap::ValidateShaders();
    SFileCloseFile(file);
}

// VERIFY: 2b31b0e9 manual
// OFFSET: 0x007BFCE0
void CMap::Load(const char* mapName, int32_t zoneID) {
    CMap::s_unk_CE049C = false;

    uint32_t length = SStrCopy(CMap::mapPath, "World\\Maps\\", STORM_MAX_STR);
    SStrCopy(&CMap::mapPath[length], mapName, STORM_MAX_STR);
    SStrCopy(CMap::mapName, mapName, STORM_MAX_STR);
    SStrPrintf(CMap::wdtFilename, sizeof(CMap::wdtFilename), "%s\\%s.wdt", CMap::mapPath, CMap::mapName);

    CMap::s_mapLight = CMap::CreateLight(1, 0);
    CMap::s_mapLight->m_light.SetLightType(M2LIGHT_DIRECT);
    CMap::EnableLight(CMap::s_mapLight);
    CMap::UpdateLight(CMap::s_mapLight);
    CMap::PurgeMaps();
    CMapObj::ClearCache(1);
    CMap::PurgeUnkCache();

    s_mapId = zoneID;
    CMap::bActive = 1;
    CMap::bDungeon = 0;
    CMap::bPreload = 1;

    int32_t streaming = SFile::IsStreamingMode();
    CMap::s_isStreamingMode = SFile::IsTrial() | streaming;
    bool wait = CMap::s_isStreamingMode == 0;

    CMap::s_lowDetail.Load(CMap::mapPath, CMap::mapName);
    CMap::LoadWdt();
    CMap::LoadTex();
    DayNight::LoadMap(zoneID);
    CMap::PrepareUpdate(false);

    if (wait) {
        AsyncFileReadWaitAll();
    }

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(1.0f, World::s_loadProgressParam);
    }

    CMap::bPreload = 0;
    World::s_loadProgressCallback = nullptr;
    CWorld::status.m_prepareAll = 1;
}

// VERIFY: 512418e2 auto
// OFFSET: 0x007BFF20
CMapObj* CMap::AllocMapObj() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObj* mapObj = new (object) CMapObj();
    mapObj->m_objectIndex = memHandle;

    return mapObj;
}

// VERIFY: 3e380871 auto
// OFFSET: 0x007BFFE0
CMapObjGroup* CMap::AllocMapObjGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjGroupHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObjGroup* mapObjGroup = new (object) CMapObjGroup();
    mapObjGroup->m_objectIndex = memHandle;

    return mapObjGroup;
}

// VERIFY: 8cfa8101 manual
// OFFSET: 0x007C00A0
void CMap::FreeArea(CMapArea* area) {
    area->m_lameAssLink.Unlink();
    area->~CMapArea();
    ObjectFree(*CMap::areaHeap, area->m_objectIndex);
}

// VERIFY: 53440967 manual
// OFFSET: 0x007C0110
void CMap::FreeAreaMed(CMapAreaMed* areaMed) {
    areaMed->m_lameAssLink.Unlink();
    areaMed->~CMapAreaMed();
    ObjectFree(*CMap::areaMedHeap, areaMed->m_objectIndex);
}

// VERIFY: 27b1c9a8 manual
// OFFSET: 0x007C0180
void CMap::FreeChunk(CMapChunk* chunk) {
    chunk->m_lameAssLink.Unlink();
    chunk->~CMapChunk();
    ObjectFree(*CMap::chunkHeap, chunk->m_objectIndex);
}

// VERIFY: d1b91d2a auto
// OFFSET: 0x007C01F0
CMapDoodadDef* CMap::AllocDoodadDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::doodadDefHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapDoodadDef* doodadDef = new (object) CMapDoodadDef();
    doodadDef->m_objectIndex = memHandle;

    return doodadDef;
}

// VERIFY: fe23a5b5 auto
// OFFSET: 0x007C03E0
CMapObjDef* CMap::AllocMapObjDef() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjDefHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapObjDef* mapObjDef = new (object) CMapObjDef();
    mapObjDef->m_objectIndex = memHandle;

    return mapObjDef;
}

// VERIFY: 37cad926 manual
// OFFSET: 0x007C04A0
void CMap::FreeChunkLiquid(CChunkLiquid* liquid) {
    liquid->m_link.Unlink();
    liquid->~CChunkLiquid();
    ObjectFree(*CMap::chunkLiquidHeap, liquid->m_objectIndex);
}

// VERIFY: 03529b4c manual
// OFFSET: 0x007C0500
CMapRenderChunk* CMap::AllocRenderChunk() {
    CMapRenderChunk* renderChunk = CMap::s_mapRenderChunkFreeList.Head();

    if (!renderChunk) {
        renderChunk = CMap::s_mapRenderChunkFreeList.NewNode(2, 0, 0);
        renderChunk->m_renderChunkLink.Unlink();
        return renderChunk;
    }

    renderChunk->m_renderChunkLink.Unlink();
    new (renderChunk) CMapRenderChunk();

    return renderChunk;
}

// VERIFY: d11dd1c9 auto
// OFFSET: 0x007C0670
CMapEntity* CMap::AllocEntity(int32_t linkToHead) {
    uint32_t memHandle;
    void* object = nullptr;
    CMapEntity* entity;

    if (ObjectAlloc(*CMap::entityHeap, &memHandle, &object, false)) {
        entity = new (object) CMapEntity();
        entity->m_objectIndex = memHandle;
    } else {
        entity = nullptr;
    }

    if (linkToHead) {
        CMap::entityList.LinkToHead(entity);
    } else {
        CMap::entityList.LinkToTail(entity);
    }

    return entity;
}

// VERIFY: 3adb704b manual
// OFFSET: 0x007C0750
CMapBaseObjLink* CMap::AllocBaseObjLink(CMapBaseObj* baseObj) {
    uint32_t memHandle;
    void* object = nullptr;
    CMapBaseObjLink* link;

    if (ObjectAlloc(*CMap::baseObjLinkHeap, &memHandle, &object, false)) {
        link = new (object) CMapBaseObjLink;
        link->m_objectIndex = memHandle;
    } else {
        link = nullptr;
    }

    baseObj->m_refCount++;
    link->m_owner = baseObj;
    link->m_ref = nullptr;
    baseObj->m_parentLinkList.LinkToTail(link);

    return link;
}

// VERIFY: e4e64c5e auto
// OFFSET: 0x007C07C0
CMapArea* CMap::AllocArea() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::areaHeap, &memHandle, &object, false)) {
        CMap::s_unk_AEED8C.LinkToTail(nullptr);
        return nullptr;
    }

    CMapArea* area = new (object) CMapArea();
    area->m_objectIndex = memHandle;
    CMap::s_unk_AEED8C.LinkToTail(area);

    return area;
}

// VERIFY: 2acaa66b auto
// OFFSET: 0x007C0830
CMapChunk* CMap::AllocMapChunk() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::chunkHeap, &memHandle, &object, false)) {
        CMap::s_mapChunkList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapChunk* chunk = new (object) CMapChunk();
    chunk->m_objectIndex = memHandle;
    CMap::s_mapChunkList.LinkToTail(chunk);

    return chunk;
}

// VERIFY: 430261e3 auto
// OFFSET: 0x007C08A0
CMapLight* CMap::AllocLight() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::lightHeap, &memHandle, &object, false)) {
        CMap::lightList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapLight* light = new (object) CMapLight();
    light->m_objectIndex = memHandle;
    CMap::lightList.LinkToTail(light);

    return light;
}

// VERIFY: 7128fd21 auto
// OFFSET: 0x007C0910
CMapObjDefGroup* CMap::AllocMapObjDefGroup() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::mapObjDefGroupHeap, &memHandle, &object, false)) {
        CMap::mapObjDefGroupList.LinkToTail(nullptr);
        return nullptr;
    }

    CMapObjDefGroup* group = new (object) CMapObjDefGroup();
    group->m_objectIndex = memHandle;
    CMap::mapObjDefGroupList.LinkToTail(group);

    return group;
}

// VERIFY: ecff775f auto
// OFFSET: 0x007C0980
CChunkLiquid* CMap::AllocChunkLiquid() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::chunkLiquidHeap, &memHandle, &object, false)) {
        CMap::s_chunkLiquidList.LinkToTail(nullptr);
        return nullptr;
    }

    CChunkLiquid* liquid = new (object) CChunkLiquid();
    liquid->m_objectIndex = memHandle;
    CMap::s_chunkLiquidList.LinkToTail(liquid);

    return liquid;
}

// VERIFY: c9309208 manual
// OFFSET: 0x007C09F0
void CMap::FreeBaseObjLink(CMapBaseObjLink* link) {
    link->m_ownerLink.Unlink();
    link->m_refLink.Unlink();
    link->m_owner->m_refCount--;
    link->m_ref = nullptr;
    link->m_owner = nullptr;
    link->~CMapBaseObjLink();
    ObjectFree(*CMap::baseObjLinkHeap, link->m_objectIndex);
}

// VERIFY: c37a9deb auto
// OFFSET: 0x007C0A90
CMapAreaLow* CMap::AllocAreaLow() {
    uint32_t memHandle;
    void* object = nullptr;

    if (!ObjectAlloc(*CMap::areaLowHeap, &memHandle, &object, false)) {
        return nullptr;
    }

    CMapAreaLow* areaLow = new (object) CMapAreaLow();
    areaLow->m_objectIndex = memHandle;

    return areaLow;
}

// VERIFY: 391a570d auto
// OFFSET: 0x007C15F0
void CMap::ClassifyStaticEntityLink(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, uint32_t faceInfo, C3Vector* pos) {
    CMapObjGroup* group = mapObjDef->m_owner->GetGroup(mapObjDefGroup->m_groupNum, false);

    if (!group) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else if (group->m_flags & 0x8) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else if (group->m_flags & 0x40) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    } else {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_INTERIOR);
    }

    if (entity->m_flags & MAPOBJ_FLAG_INTERIOR) {
        entity->QueryInteriorLighting(mapObjDef, mapObjDefGroup->m_groupNum, reinterpret_cast<uint16_t*>(&faceInfo), pos);
    }
}

// VERIFY: d84c44fe manual
// OFFSET: 0x007C1660
bool CMap::LinkStaticEntityGetChunk(C3Vector& pos, float* outHeight, CMapChunk** outChunk) {
    *outChunk = nullptr;

    int32_t cellX = CMath::fint_(-(pos.y - 17066.666f) * 0.24f - 0.5f);
    int32_t cellY = CMath::fint_(0.24f * -(pos.x - 17066.666f) - 0.5f);

    CMapArea* area = CMap::m_areaTable[64 * ((cellY >> 7) & 0x3F) + ((cellX >> 7) & 0x3F)];

    if (!area || area->m_asyncObject) {
        return false;
    }

    CMapChunk* chunk = area->m_mapChunks[((cellX >> 3) & 0xF) + 16 * ((cellY >> 3) & 0xF)];

    if (!chunk) {
        return false;
    }

    *outChunk = chunk;

    return CMap::GetHeightTerrain(chunk, pos, cellX, cellY, outHeight);
}

// VERIFY: 4e801f8f manual
// OFFSET: 0x007C1DC0
void CMap::LinkIntersectMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, C3Vector& start, C3Vector& end, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    CMapObj* mapObj = mapObjDef->m_owner;
    CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->m_groupNum, false);

    if (!group) {
        return;
    }

    int32_t interiorFace = -1;
    int32_t groundFace = -1;

    C3Segment seg;
    seg.b = start;
    seg.t = end;

    int16_t interior = !(group->m_flags & 0x8);

    if (group->GetFacesForLinking(seg, &interiorHit->t, &interiorFace, &groundHit->t, &groundFace)) {
        if (interiorFace != -1) {
            interiorHit->def = mapObjDef;
            interiorHit->group = mapObjDefGroup;
            interiorHit->faceIndex = interiorFace;
            interiorHit->interior = interior;
        }

        if (groundFace != -1) {
            groundHit->faceIndex = groundFace;
            groundHit->def = mapObjDef;
            groundHit->group = mapObjDefGroup;
            groundHit->interior = interior;
        }
    }

    if (!interior) {
        return;
    }

    float t = 1.05f;
    int32_t groups[2];

    if (mapObj->VectorIntersectPortals(mapObjDefGroup->m_groupNum, seg, &t, groups) && t - interiorHit->t < 0.0001f) {
        interiorHit->def = mapObjDef;
        CMapObjDefGroup* portalGroup = mapObjDef->m_defGroups[groups[0]];
        interiorHit->t = t;
        interiorHit->group = portalGroup;
        interiorHit->faceIndex = -1;
        interiorHit->interior = !(mapObj->GetGroup(portalGroup->m_groupNum, false)->m_flags & 0x8);
    }
}

// VERIFY: 86bfa752 auto
// OFFSET: 0x007C1FF0
void CMap::LinkObjectToMapObjDefGroup(CMapStaticEntity* object, CMapObjDefGroup* mapObjDefGroup) {
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(object);
    link->m_ref = mapObjDefGroup;

    if (object->m_type & 0x20) {
        if (object->unk_007C & 0x2) {
            mapObjDefGroup->m_entityLinkList.LinkToHead(link);
            return;
        }

        mapObjDefGroup->m_entityLinkList.LinkToTail(link);
    } else if (object->m_type & 0x40) {
        mapObjDefGroup->m_doodadDefLinkList.LinkToTail(link);
    }
}

// VERIFY: d4db675a auto
// OFFSET: 0x007C2040
bool CMap::LinkObjectToMapExterior(CMapStaticEntity* object) {
    bool linked = false;

    if (CMap::bDungeon) {
        return false;
    }

    float maxX = -(object->m_bbox.t.x - 17066.666f);
    float maxY = -(object->m_bbox.t.y - 17066.666f);
    float minX = -(object->m_bbox.b.x - 17066.666f);

    int32_t lastCol = CMath::fint_(-(object->m_bbox.b.y - 17066.666f) * 0.03f - 0.5f);
    int32_t lastRow = CMath::fint_(minX * 0.03f - 0.5f);
    int32_t firstCol = CMath::fint_(maxY * 0.03f - 0.5f);
    int32_t firstRow = CMath::fint_(maxX * 0.03f - 0.5f);

    for (int32_t row = firstRow; row <= lastRow; row++) {
        for (int32_t col = firstCol; col <= lastCol; col++) {
            CMapArea* area = CMap::m_areaTable[(((row >> 4) & 0x3F) << 6) + ((col >> 4) & 0x3F)];

            if (!area || area->m_asyncObject) {
                continue;
            }

            CMapChunk* chunk = area->m_mapChunks[(col & 0xF) + 16 * (row & 0xF)];

            if (!chunk || chunk->m_bbox.b.z > object->m_bbox.t.z) {
                continue;
            }

            CMapBaseObjLink* link = CMap::AllocBaseObjLink(object);
            link->m_ref = chunk;

            if (object->m_type & 0x20) {
                if (object->unk_007C & 0x2) {
                    chunk->m_entityLinkList.LinkToHead(link);
                } else {
                    chunk->m_entityLinkList.LinkToTail(link);
                }
            } else if (object->m_type & 0x40) {
                chunk->m_doodadDefLinkList.LinkToTail(link);
            }

            object->m_flags = static_cast<MapObjFlag>(object->m_flags | MAPOBJ_FLAG_EXTERIOR);
            linked = true;
        }
    }

    return linked;
}

// VERIFY: 4d233b7e manual
// OFFSET: 0x007C25D0
bool CMap::LinkIntersectMapObjDef(CMapObjDef* mapObjDef, C3Vector& start, C3Vector& end, C3Vector& mid, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    CMapObj* mapObj = mapObjDef->m_owner;

    if (!mapObj) {
        return false;
    }

    C3Vector localStart = start * mapObjDef->m_invMat;
    C3Vector localEnd = end * mapObjDef->m_invMat;
    C3Vector localMid = mid * mapObjDef->m_invMat;

    uint32_t noDepthSort = mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT;

    for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->m_owner);
        uint32_t groupFlags = mapObj->GetGroupFlags(group->m_groupNum);

        if (!(groupFlags & 0x410080) && mapObj->TestGroupBounds(localStart, localEnd, group->m_groupNum) && ((!noDepthSort && (groupFlags & 0x8)) || mapObj->TestGroupBounds(localMid, group->m_groupNum))) {
            CMap::LinkIntersectMapObjDefGroup(mapObjDef, group, localStart, localEnd, &interiorHit[noDepthSort == 0], &groundHit[noDepthSort == 0]);
        }
    }

    return true;
}

// VERIFY: c04d5774 manual
// OFFSET: 0x007C2700
bool CMap::LinkIntersectMapObjDefs(C3Vector& start, C3Vector& end, C3Vector& mid, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit, CMapChunk* chunk) {
    groundHit[0].t = 1.05f;
    groundHit[0].def = nullptr;
    groundHit[1].t = 1.05f;
    groundHit[1].def = nullptr;
    interiorHit[0].t = 1.05f;
    interiorHit[0].def = nullptr;
    interiorHit[1].t = 1.05f;
    interiorHit[1].def = nullptr;

    if (chunk) {
        for (CMapBaseObjLink* link = chunk->m_mapObjDefLinkList.Head(); link; link = chunk->m_mapObjDefLinkList.Next(link)) {
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) && mapObjDef->TestAABox(start, end)) {
                CMap::LinkIntersectMapObjDef(mapObjDef, start, end, mid, interiorHit, groundHit);
            }
        }
    } else {
        for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) && mapObjDef->TestAABox(start, end)) {
                CMap::LinkIntersectMapObjDef(mapObjDef, start, end, mid, interiorHit, groundHit);
            }
        }
    }

    if (!interiorHit[0].def && groundHit[0].def) {
        interiorHit[0] = groundHit[0];
    }

    if (!interiorHit[1].def && groundHit[1].def) {
        interiorHit[1] = groundHit[1];
    }

    if (!interiorHit[0].def) {
        if (!interiorHit[1].def) {
            return false;
        }

        interiorHit[0] = interiorHit[1];
        groundHit[0] = groundHit[1];
        interiorHit[1].def = nullptr;
        groundHit[1].def = nullptr;
    }

    if (!groundHit[0].def) {
        groundHit[0] = interiorHit[0];
        groundHit[0].faceIndex = -1;
    }

    if (!groundHit[1].def) {
        groundHit[1] = interiorHit[1];
        groundHit[1].faceIndex = -1;
    }

    return true;
}

// VERIFY: fbe3b25a manual
// OFFSET: 0x007C28F0
void CMap::LinkStaticEntity(CMapStaticEntity* entity, C3Vector& top, C3Vector& bottom, C3Vector& mid, int32_t& outInterior, int32_t& outHit, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    float height = 100000.0f;
    float terrainT = 2.0f;

    interiorHit[0].def = nullptr;
    groundHit[0].def = nullptr;
    interiorHit[1].def = nullptr;
    groundHit[1].def = nullptr;

    CMapChunk* chunk = nullptr;
    bool terrain = false;

    if (!CMap::bDungeon) {
        terrain = CMap::LinkStaticEntityGetChunk(top, &height, &chunk);
        terrainT = (top.z - height) * 0.001f;

        if (terrainT < 0.0f) {
            terrain = false;
        }
    }

    bool hit;

    if (entity->m_flags & MAPOBJ_FLAG_NO_MAPOBJ_LINK) {
        hit = false;
    } else {
        hit = CMap::LinkIntersectMapObjDefs(top, bottom, mid, interiorHit, groundHit, nullptr);
    }

    if (!terrain && !hit) {
        top = entity->m_vec2;
        bottom = entity->m_vec2;
        bottom.z = bottom.z + 1000.0f;
        CMap::LinkIntersectMapObjDefs(top, bottom, mid, interiorHit, groundHit, nullptr);
    }

    if (entity->unk_007C & 0x2000) {
        interiorHit[1].def = nullptr;
        groundHit[1].def = nullptr;
    }

    if (terrain) {
        if (terrainT < interiorHit[0].t) {
            interiorHit[0].def = nullptr;
            groundHit[0].def = nullptr;
        }

        if (terrainT < interiorHit[1].t) {
            interiorHit[1].def = nullptr;
            groundHit[1].def = nullptr;
        }
    }

    outInterior = 0;
    outHit = 0;

    if (interiorHit[0].def) {
        outHit = 1;
    }

    if (interiorHit[1].def) {
        outHit = 1;
    }

    if (interiorHit[0].def) {
        outInterior = interiorHit[0].interior;
    } else if (interiorHit[1].def) {
        outInterior = interiorHit[1].interior;
    }
}

// VERIFY: c4018711 auto
// OFFSET: 0x007C2A70
void CMap::LinkStaticEntitySingle2(CMapStaticEntity* entity) {
    C3Vector top(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 0.1f);
    C3Vector bottom(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - 1000.0f);

    int32_t index = 0;
    int32_t interior = 0;
    int32_t hit = 0;
    MapObjIntersectData interiorHit[2];
    MapObjIntersectData groundHit[2];

    CMap::LinkStaticEntity(entity, top, bottom, top, interior, hit, interiorHit, groundHit);

    uint32_t isEntity = entity->m_type & 0x20;
    uint32_t groundType = 0xFFFFFFFF;

    if (!hit) {
        CMap::LinkObjectToMapExterior(entity);

        if (isEntity) {
            static_cast<CMapEntity*>(entity)->m_groundType = CMap::QueryGroundTypeTerrain(&entity->m_position, &groundType) ? groundType : -1;
        }

        entity->m_flags = static_cast<MapObjFlag>((entity->m_flags & ~MAPOBJ_FLAG_IN_MAPOBJ) | MAPOBJ_FLAG_EXTERIOR);
        return;
    }

    if (!groundHit[0].def) {
        index = 1;
    }

    if (interiorHit[0].def) {
        CMap::LinkObjectToMapObjDefGroup(entity, interiorHit[0].group);
    }

    if (interiorHit[1].def) {
        CMap::LinkObjectToMapObjDefGroup(entity, interiorHit[1].group);
    }

    if (isEntity) {
        static_cast<CMapEntity*>(entity)->m_groundType = groundHit[index].def->GetGroundType(groundHit[index].group->m_groupNum, groundHit[index].faceIndex);
    }

    entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_IN_MAPOBJ);

    if (!interior) {
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
        return;
    }

    entity->unk_007C |= 0x1;

    C3Vector pos(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - (bottom.z - top.z) * groundHit[index].t);
    CMap::ClassifyStaticEntityLink(entity, groundHit[index].def, groundHit[index].group, groundHit[index].faceIndex, &pos);
}

// VERIFY: 68d8dacb manual
// OFFSET: 0x007C2BF0
void CMap::LinkStaticEntityMultiple2ToMapObjDefExterior(CMapStaticEntity* entity) {
    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
            continue;
        }

        CMapObj* mapObj = mapObjDef->m_owner;

        if (!mapObj || !mapObj->m_isGroupLoaded) {
            continue;
        }

        CAaBox box;
        CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, box);

        if (!mapObj->TestBounds(box)) {
            continue;
        }

        for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
            CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->m_owner);
            uint32_t groupFlags = mapObj->GetGroupFlags(group->m_groupNum);

            if (!(groupFlags & 0x410080) && (groupFlags & 0x8) && mapObj->TestGroupBounds(box, group->m_groupNum, 1)) {
                CMap::LinkObjectToMapObjDefGroup(entity, group);
            }
        }
    }
}

// VERIFY: 3091ac75 manual
// OFFSET: 0x007C2D30
void CMap::LinkStaticEntityMultiple2ToMapObjDefInterior(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    CMapObj* mapObj = mapObjDef->m_owner;

    if (!mapObj || !mapObj->m_isGroupLoaded) {
        return;
    }

    CAaBox box;
    CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, box);

    if (!mapObj->TestBounds(box)) {
        return;
    }

    CMap::LinkObjectToMapObjDefGroup(entity, mapObjDefGroup);

    for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->m_owner);

        if (group != mapObjDefGroup && !(mapObj->GetGroupFlags(group->m_groupNum) & 0x410088) && mapObj->TestGroupBounds(box, group->m_groupNum, 1)) {
            CMap::LinkObjectToMapObjDefGroup(entity, group);
        }
    }
}

// VERIFY: e348a76f auto
// OFFSET: 0x007C2E70
void CMap::LinkStaticEntityMultiple2(CMapStaticEntity* entity) {
    C3Vector top(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 4.0f);
    C3Vector bottom(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - 1000.0f);
    C3Vector mid(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 0.15000001f);

    float boxTop = entity->m_bbox.t.z + 0.1f;

    if (boxTop < top.z) {
        top.z = boxTop;
    }

    int32_t interior = 0;
    int32_t hit;
    MapObjIntersectData interiorHit[2];
    MapObjIntersectData groundHit[2];

    CMap::LinkStaticEntity(entity, top, bottom, mid, interior, hit, interiorHit, groundHit);

    if (interior) {
        CMap::LinkStaticEntityMultiple2ToMapObjDefInterior(entity, interiorHit[0].def, interiorHit[0].group);
        entity->unk_007C |= 0x1;

        C3Vector pos(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - (bottom.z - top.z) * groundHit[0].t);
        CMap::ClassifyStaticEntityLink(entity, groundHit[0].def, groundHit[0].group, groundHit[0].faceIndex, &pos);
    } else {
        CMap::LinkStaticEntityMultiple2ToMapObjDefExterior(entity);
        CMap::LinkObjectToMapExterior(entity);
        entity->m_flags = static_cast<MapObjFlag>(entity->m_flags | MAPOBJ_FLAG_EXTERIOR);
    }
}

// VERIFY: d35a4a66 manual
// OFFSET: 0x007C2F80
void CMap::LinkStaticEntity(CMapStaticEntity* entity) {
    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    while (link) {
        CMapBaseObjLink* next = entity->m_parentLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    if (entity->m_type & 0x20) {
        static_cast<CMapEntity*>(entity)->m_groundType = -1;
    }

    if (entity->unk_007C & 0x2000) {
        CMap::LinkStaticEntityMultiple2(entity);
    } else {
        CMap::LinkStaticEntitySingle2(entity);
    }
}

// VERIFY: e2dae8ec manual
// OFFSET: 0x007C3730
void CMap::PurgeMaps() {
    CMapBaseObjLink* link = CMap::s_unk_ADFBEC.Head();

    while (link) {
        CMapBaseObjLink* next = CMap::s_unk_ADFBEC.Next(link);
        CMapArea* area = static_cast<CMapArea*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        CMap::m_areaTable[64 * area->m_index.y + area->m_index.x] = nullptr;
        area->Purge();
        CMap::FreeArea(area);
        link = next;
    }

    for (uint32_t i = 0; i < 4096; i++) {
        if (CMap::unk[i]) {
            CMap::FreeAreaMed(CMap::unk[i]);
            CMap::unk[i] = nullptr;
        }
    }

    CMap::s_lowDetail.Purge();
}

// VERIFY: 6db55b9e manual
// OFFSET: 0x007D9990
CTexture* CMap::LoadTexture(const char* fileName) {
    CStatus status;
    CTexture* texture = TextureCreate(fileName, CGxTexFlags(GxTex_LinearMipLinear, 1, 1, 0, 0, 0, 1), &status, 0);
    SysMsgAdd(&status);

    if (!texture) {
        DebugLog("Failed to load texture: %s\n");
    }

    return texture;
}

// VERIFY: fedf0bee auto
// OFFSET: 0x007D9A20
void CMap::LoadArea(CMapArea* area) {
    char fileName[256];
    SStrPrintf(fileName, sizeof(fileName), "%s\\%s_%d_%d.adt", CMap::mapPath, CMap::mapName, area->m_index.x, area->m_index.y);
    area->Load(fileName);
}

// VERIFY: a0b22da5 auto
// OFFSET: 0x007D9A70
CMapArea* CMap::PrepareArea(int32_t areaIndexX, int32_t areaIndexY) {
    CMapArea* area = CMap::AllocArea();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(area);
    CMap::s_unk_ADFBEC.LinkToTail(link);

    area->m_tileChunkIndex.x = 16 * areaIndexX;
    area->m_index.y = areaIndexY;
    area->m_flags = static_cast<MapObjFlag>(0);
    area->m_index.x = areaIndexX;
    area->m_tileChunkIndex.y = 16 * areaIndexY;

    area->m_topLeft2.x = 17066.666f - area->m_tileChunkIndex.y * 33.333332f;
    area->m_bounds.t.x = area->m_topLeft2.x;
    area->m_topLeft2.y = area->m_tileChunkIndex.x * -33.333332f + 17066.666f;
    area->m_bounds.t.y = area->m_topLeft2.y;
    area->m_bounds.b.x = area->m_topLeft2.x - 533.33331f;
    area->m_bounds.b.y = area->m_topLeft2.y - 533.33331f;

    CMap::m_areaTable[64 * areaIndexY + areaIndexX] = area;

    return area;
}

// OFFSET: 0x007D9B10
CMapLight::CMapLight() {
    this->m_sphere.r = 0.0f;
    this->m_type |= 0x80;
    this->unk_00CC = 0.0f;
    this->m_radius = 0.0f;
    this->unk_00D0 = 0;
    this->unk_00C4 = 0.0f;
}

// VERIFY: 0aba9189 manual
// OFFSET: 0x007D9BD0
CMapLight* CMap::CreateLight(uint8_t a1, uint8_t a2) {
    CMapLight* light = CMap::AllocLight();
    light->m_flags = static_cast<MapObjFlag>(0);
    light->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_sphere.r = 0.0f;
    light->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_bbox.b = C3Vector(0.0f, 0.0f, 0.0f);
    light->unk_00CC = 0.0f;
    light->m_radius = 0.0f;
    light->m_bbox.t = C3Vector(0.0f, 0.0f, 0.0f);
    light->unk_00C4 = 0.0f;
    light->unk_00D0 = a1;
    light->unk_00D1 = a2;

    return light;
}

// VERIFY: e4a742da manual
// OFFSET: 0x007D9C80
void CMap::UpdateLightBounds(CMapLight* light) {
    float radius = light->m_radius;

    if (light->unk_00CC == 0.0f) {
        radius = 5.0f;
    }

    light->m_radius = radius;

    C3Vector& pos = light->m_light.m_pos;
    light->m_position = pos;
    light->m_bbox.b = C3Vector(pos.x - radius, pos.y - radius, pos.z - radius);
    light->m_bbox.t = C3Vector(pos.x + radius, pos.y + radius, pos.z + radius);
    light->m_sphere.c = pos;
    light->m_sphere.r = radius;
}

// VERIFY: 66f3244f auto
// OFFSET: 0x007D9D50
void CMap::EnableLight(CMapLight* light) {
    light->m_flags = static_cast<MapObjFlag>(light->m_flags & ~0x20);
    light->m_light.SetVisible(1);
}

// VERIFY: 8d2bb286 auto
// OFFSET: 0x007D9DE0
void CMap::LinkLightToChunks(CMapLight* light) {
    int32_t maxY = CMath::fint_(-(light->m_bbox.b.y - 17066.666f) * 0.03f - 0.5f);
    int32_t maxX = CMath::fint_(-(light->m_bbox.b.x - 17066.666f) * 0.03f - 0.5f);
    int32_t minY = CMath::fint_(-(light->m_bbox.t.y - 17066.666f) * 0.03f - 0.5f);
    int32_t minX = CMath::fint_(-(light->m_bbox.t.x - 17066.666f) * 0.03f - 0.5f);

    for (int32_t x = minX; x <= maxX; x++) {
        for (int32_t y = minY; y <= maxY; y++) {
            CMapArea* area = CMap::m_areaTable[(((x >> 4) & 0x3F) << 6) + ((y >> 4) & 0x3F)];

            if (!area || area->m_asyncObject) {
                continue;
            }

            CMapChunk* chunk = area->m_mapChunks[(y & 0xF) + 16 * (x & 0xF)];

            if (!chunk) {
                continue;
            }

            CMapBaseObjLink* link = CMap::AllocBaseObjLink(light);
            link->m_ref = chunk;
            chunk->m_lightLinkList.LinkToTail(link);
            chunk->UpdateLights();
        }
    }
}

// VERIFY: 75a5e857 manual
// OFFSET: 0x007D9F90
void CMap::LinkLightToMapObjDefs(CMapLight* light) {
    CAaBox box;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if (mapObjDef->m_flags & 0x20) {
            continue;
        }

        if (light->m_bbox.t.x >= mapObjDef->m_bbox.b.x && light->m_bbox.t.y >= mapObjDef->m_bbox.b.y && light->m_bbox.t.z >= mapObjDef->m_bbox.b.z
            && light->m_bbox.b.x <= mapObjDef->m_bbox.t.x && light->m_bbox.b.y <= mapObjDef->m_bbox.t.y && light->m_bbox.b.z <= mapObjDef->m_bbox.t.z) {
            CMapObj* mapObj = mapObjDef->m_owner;

            if (mapObj && mapObj->TestBounds(box)) {
                for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
                    CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->m_owner);

                    if (!(group->m_flags & 0x2) && light->m_bbox.Intersects(group->m_bbox)) {
                        CMapBaseObjLink* lightLink = CMap::AllocBaseObjLink(light);
                        lightLink->m_ref = group;
                        group->m_lightLinkList.LinkToTail(lightLink);
                        group->UpdateLights();
                    }
                }
            }
        }
    }
}

// VERIFY: 7439bd46 manual
// OFFSET: 0x007DA100
void CMap::UpdateLight(CMapLight* light) {
    if (!light->m_parentLinkList.Head() && !light->m_light.m_type) {
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(light);
        link->m_ref = nullptr;
        CMap::s_unk_AF16CC.LinkToTail(link);
    }

    if (CMap::bActive && light->m_light.m_type) {
        CMapBaseObjLink* link = light->m_parentLinkList.Head();

        while (link) {
            CMapBaseObjLink* next = light->m_parentLinkList.Next(link);
            CMap::FreeBaseObjLink(link);
            link = next;
        }

        CMap::UpdateLightBounds(light);
        CMap::LinkLightToMapObjDefs(light);
        CMap::LinkLightToChunks(light);
    }
}
