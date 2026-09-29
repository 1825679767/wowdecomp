#include "Map.h"
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include <cstring>
#include <cmath>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "WoW/Source/WorldClient/FVBBList.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapLoad.h"
#include "WoW/Source/WorldClient/MapLowDetail.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/WaterRipple.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldView.h"

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

// STATIC: 0x00CF497C
static CMapFootprint unk_CF497C[144][4];

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

// STATIC: 0x00A3FB88
uint16_t CMap::s_2bitSplatMask[8] = { 0x3, 0xC, 0x30, 0xC0, 0x300, 0xC00, 0x3000, 0xC000 };

// STATIC: 0x00A3FB98
uint32_t CMap::s_2bitSplatShft[8] = { 0, 2, 4, 6, 8, 10, 12, 14 };

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
HASHKEY_NONE CMap::s_mapObjDefHashKey;

// STATIC: 0x00ADFC04
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::mapObjDefLinkList = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00AF16CC
TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> > CMap::s_unk_AF16CC = TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>();

// STATIC: 0x00ADFC1C
TSList<CMapChunk, TSGetExplicitLink<CMapChunk> > CMap::s_mapChunkSortList = TSExplicitList<CMapChunk, offsetof(CMapChunk, m_sortListLink)>();

// STATIC: 0x00D2545C
TSHashTable<CMapDoodadDef, HASHKEY_DWORD> CMap::doodadDefHash;

// STATIC: 0x00AEEDE0
TSExplicitList<CDestructibleProxy, offsetof(CDestructibleProxy, m_link)> CMap::s_destructibleProxyList;

// STATIC: 0x00CE04BC
int32_t CMap::mapGetFacetsCount;

// STATIC: 0x00CE04C0
int32_t CMap::bspRecurseCount;

// STATIC: 0x00ADFC40
TSList<CDetailDoodadInst, TSGetExplicitLink<CDetailDoodadInst> > CMap::s_detailDoodadUpdateList = TSExplicitList<CDetailDoodadInst, offsetof(CDetailDoodadInst, m_link)>();

// VERIFY: 738eb1a4 manual
// OFFSET: 0x0079B440
CMapArea* CMap::GetLoadedArea(int32_t areaX, int32_t areaY) {
    CMapArea* area = CMap::m_areaTable[64 * areaY + areaX];

    if (area && area->m_asyncObject) {
        return nullptr;
    }

    return area;
}

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

// VERIFY: e651a57d manual
// OFFSET: 0x0079E5C0
CGxShader* CMap::GetTerrainPixelShader(int32_t bigAlpha, int32_t layers, int32_t shadowMode, int32_t a4, int32_t a5) {
    int32_t layerIndex = layers - 1;

    switch (shadowMode) {
        case 0:
            return CMap::pixelShader_Terrain1[16 * a5 + 8 * bigAlpha + 2 * layerIndex + a4];

        case 1:
            if (g_theGxDevicePtr->Caps().m_shaderTargets[4] == 3 || g_theGxDevicePtr->Caps().m_shaderTargets[4] == 12) {
                return CMap::pixelShader_Terrain2[16 * a5 + 8 * bigAlpha + 2 * layerIndex + a4];
            }

            return CMap::pixelShader_Terrain3[48 * a5 + 24 * bigAlpha + 2 * layerIndex + a4];

        case 2:
        case 3:
            return CMap::pixelShader_Terrain3[48 * a5 - 8 + 24 * bigAlpha + 8 * shadowMode + 2 * layerIndex + a4];

        default:
            return nullptr;
    }
}

// VERIFY: 2fffff6e manual
// OFFSET: 0x0079E730
void CMap::ClearChunkDetailDoodads() {
    for (CMapChunk* chunk = CMap::s_mapChunkList.Head(); chunk; chunk = CMap::s_mapChunkList.Next(chunk)) {
        if (chunk->m_detailDoodadInst) {
            CDetailDoodad::FreeInst(chunk->m_detailDoodadInst);
            chunk->m_detailDoodadInst = nullptr;
        }
    }
}

// VERIFY: 77579752 manual
// OFFSET: 0x0079E780
void CMap::ClearAllRenderChunks() {
    for (CMapChunk* chunk = CMap::s_mapChunkList.Head(); chunk; chunk = CMap::s_mapChunkList.Next(chunk)) {
        if (chunk->m_renderChunk) {
            chunk->m_renderChunk->FreeBuf();
        }
    }
}

// VERIFY: 9b396a3d manual
// OFFSET: 0x0079EFF0
void CDestructibleProxy::FindOrCreate(CDestructibleProxy* proxy) {
    if (!proxy->m_uniqueId) {
        CMap::s_destructibleProxyList.LinkToHead(proxy);
        return;
    }

    for (CDestructibleProxy* node = CMap::s_destructibleProxyList.Head(); !CMap::s_destructibleProxyList.IsEnd(node); node = CMap::s_destructibleProxyList.RawNext(node)) {
        if (node->m_uniqueId == proxy->m_uniqueId) {
            return;
        }
    }

    CDestructibleProxy* created = CMap::s_destructibleProxyList.NewNode(2, 0, 0);
    created->unk_0000 = proxy->unk_0000;
    created->m_sphere = proxy->m_sphere;
    created->m_uniqueId = proxy->m_uniqueId;
    CMap::s_destructibleProxyList.LinkToHead(created);
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

// VERIFY: 3f4da488 manual
// OFFSET: 0x0079F860
void CMap::FootprintDestroy() {
    for (uint32_t i = 0; i < 144; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            unk_CF497C[i][j].m_link.Unlink();
        }
    }

    for (uint32_t i = 0; i < unk_CFBE60.Count(); i++) {
        if (unk_CFBE60[i].m_texture) {
            HandleClose(reinterpret_cast<HOBJECT>(unk_CFBE60[i].m_texture));
        }
    }
}

// VERIFY: c24ffe6e manual
// OFFSET: 0x0079FA10
void CMap::ClearAllFootprints() {
    for (uint32_t i = 0; i < 144; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            unk_CF497C[i][j].m_vertices.SetCount(0);
            unk_CF497C[i][j].m_link.Unlink();
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

// VERIFY: 70d4ac15 manual
// OFFSET: 0x007A06A0
bool CMap::QueryShadow(C3Vector* pos) {
    float y = -(pos->y - 17066.666f);
    float x = -(pos->x - 17066.666f);

    if (y < 0.0f || x < 0.0f || y >= 34133.332f || x >= 34133.332f) {
        return false;
    }

    int32_t cellX = CMath::fint_(y * 0.029999999f - 0.5f);
    int32_t cellY = CMath::fint_(x * 0.029999999f - 0.5f);
    CMapArea* area = CMap::GetLoadedArea((cellX >> 4) & 0x3F, (cellY >> 4) & 0x3F);

    if (!area) {
        return false;
    }

    CMapChunk* chunk = area->m_mapChunks[16 * (cellY & 0xF) + (cellX & 0xF)];

    if (!chunk || !chunk->m_shadowMap) {
        return false;
    }

    int32_t shadowX = CMath::fint_(y * 1.92f - 0.5f) & 0x3F;
    int32_t shadowY = CMath::fint_(x * 1.92f - 0.5f) & 0x3F;
    uint8_t mamp = area->m_header->m_mampValue;
    int32_t bitX = shadowX >> mamp;

    return (chunk->m_shadowMap[(bitX >> 3) + (shadowY >> mamp) * (8 >> mamp)] & (1 << (bitX & 7))) != 0;
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

// VERIFY: c7588881 manual
// OFFSET: 0x007A09D0
bool CMap::QueryLiquidStatusMapObjsExt(C3Vector* pos, int32_t* liquidType, float* height) {
    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef->m_flags & 0x100) {
            continue;
        }

        const CAaBox& box = mapObjDef->m_bbox;

        if (box.b.x > pos->x || box.b.y > pos->y || box.b.z > pos->z || box.t.x < pos->x || box.t.y < pos->y || box.t.z < pos->z) {
            continue;
        }

        C3Vector local = *pos * mapObjDef->m_invMat;
        CMapObj* mapObj = mapObjDef->m_owner;

        if (mapObj->ContainsPointInBounds(local) && mapObj->QueryLiquidStatus(0x2000, local, liquidType, height)) {
            C3Vector level(local.x, local.y, *height);
            level *= mapObjDef->m_mat;
            *height = level.z;
            return true;
        }
    }

    return false;
}

// VERIFY: 7c5ab3e1 auto
// OFFSET: 0x007A0B00
bool CMap::QueryLiquidStatus(C3Vector* pos, int32_t* liquidType, float* height, int32_t* a4, int32_t checkGround) {
    *a4 = 0;

    if (CMap::QueryLiquidStatusMapObjsExt(pos, liquidType, height)) {
        return true;
    }

    return CMap::QueryLiquidTerrain(pos, liquidType, height, checkGround);
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


// VERIFY: 136b6340 manual
// OFFSET: 0x007A25A0
World::ObjectCollisionData::ObjectCollisionData() {
}

// VERIFY: 0869a654 manual
// OFFSET: 0x007A2760
void CMap::VectorIntersectDoodadDefs(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, uint32_t flags) {
    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if ((flags & 0x1000000) && doodadDef->m_culled) {
            continue;
        }

        if ((doodadDef->m_flags & 0x100) || !(doodadDef->m_flags & 0x80) || doodadDef->m_unkCounter == CMap::s_queryTag) {
            continue;
        }

        CM2Model* model = doodadDef->m_model;

        if (!model) {
            continue;
        }

        int32_t mode = -1;

        if (doodadDef->unk_00B8) {
            if (flags & 0x100000) {
                mode = 3;
            } else if (flags & 0x600000) {
                mode = 0;
            }
        } else if (flags & 0x1) {
            mode = 3;
        } else if (flags & 0xE) {
            mode = (flags & 0x8) ? 2 : (flags >> 24) & 1;
        }

        if (mode >= 0 && (model->m_fFlags & 1)) {
            if (!model->m_hitTestPrev) {
                model->m_hitTestPrev = &model->m_scene->m_hitTestList;
                model->m_hitTestNext = model->m_scene->m_hitTestList;
                model->m_scene->m_hitTestList = model;

                if (model->m_hitTestNext) {
                    model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
                }
            }

            model->m_hitTestMode = mode;
            model->m_hitTestOwner = doodadDef;
            model->m_hitTestGroup = 0;
        }

        doodadDef->m_unkCounter = CMap::s_queryTag;
    }
}

// VERIFY: b62820cf manual
// OFFSET: 0x007A2960
void CMap::VectorIntersectEntitys(TSList<CMapBaseObjLink, TSGetExplicitLink<CMapBaseObjLink> >* list, uint32_t flags) {
    if (!World::s_objectCollisionCallback) {
        return;
    }

    for (CMapBaseObjLink* link = list->Head(); link; link = list->Next(link)) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->m_owner);

        if ((flags & 0x1000000) && entity->m_culled) {
            continue;
        }

        if (!(entity->unk_007C & 0x2)) {
            return;
        }

        if (entity->m_unkCounter == CMap::s_queryTag || (entity->unk_007C & 0x4)) {
            continue;
        }

        World::ObjectCollisionData data;
        uint64_t guid = entity->m_funcParam64;

        if (!World::s_objectCollisionCallback(&guid, entity->m_funcParam32, &data, flags)) {
            continue;
        }

        CM2Model* model = data.m_model;

        if (!model) {
            continue;
        }

        if (model->m_fFlags & 1) {
            if (!model->m_hitTestPrev) {
                model->m_hitTestPrev = &model->m_scene->m_hitTestList;
                model->m_hitTestNext = model->m_scene->m_hitTestList;
                model->m_scene->m_hitTestList = model;

                if (model->m_hitTestNext) {
                    model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
                }
            }

            model->m_hitTestMode = (flags & 0x100000) ? 3 : 0;
            model->m_hitTestGroup = 0;
            model->m_hitTestOwner = entity;
        }

        entity->m_unkCounter = CMap::s_queryTag;
    }
}

// VERIFY: 3fe9ef3a manual
// OFFSET: 0x007A3570
bool CMap::VectorIntersectSubChunkList(C3Vector* start, C3Vector* end, float* distance, uint32_t flags, CMapChunk** hitChunk) {
    uint32_t* entry = CMap::scCollideList.m_data;
    uint32_t lastY = entry[0] & 0x2000;
    uint32_t lastX = entry[1] & 0x2000;
    uint32_t remaining = CMap::cCount;
    CMapChunk* chunk = nullptr;
    CMapChunk* bestChunk = nullptr;
    uint32_t modelFlags = flags & 0x40F0000F;

    if (modelFlags) {
        World::s_m2Scene->BeginHitTest();
    }

    float dx = end->x - start->x;
    float dy = end->y - start->y;
    float dz = end->z - start->z;
    C3Vector origin = *start;
    float invLength = 1.0f / sqrt(dz * dz + dy * dy + dx * dx);
    C3Vector dir(dx * invLength, dy * invLength, invLength * dz);
    float bestDist = *distance;

    while (remaining) {
        uint32_t cellX = entry[0];
        uint32_t cellY = entry[1];
        remaining -= 2;
        entry += 2;

        if (cellX > 0x2000 || cellY > 0x2000) {
            break;
        }

        if ((cellX & 0x1FF8) != lastX || (cellY & 0x1FF8) != lastY) {
            CMapArea* area = CMap::m_areaTable[64 * ((cellY >> 7) & 0x3F) + ((cellX >> 7) & 0x3F)];

            if (!area || area->m_asyncObject) {
                break;
            }

            chunk = area->m_mapChunks[16 * ((cellY >> 3) & 0xF) + ((cellX >> 3) & 0xF)];

            if (!chunk) {
                break;
            }

            lastX = cellX & 0x1FF8;
            lastY = cellY & 0x1FF8;
            origin.x = start->x - chunk->m_topLeftCoords.x;
            origin.y = start->y - chunk->m_topLeftCoords.y;
            origin.z = start->z - chunk->m_topLeftCoords.z;

            if (modelFlags) {
                CMap::VectorIntersectDoodadDefs(&chunk->m_doodadDefLinkList, flags);
            }

            if (flags & 0x40F00000) {
                CMap::VectorIntersectEntitys(&chunk->m_entityLinkList, flags);
            }
        }

        uint32_t subX = cellX & 7;
        uint32_t subY = cellY & 7;
        C3Ray ray;
        ray.origin = origin;
        ray.dir = dir;

        if (flags & 0x100) {
            float t = 3.4028235e38f;

            if (chunk->Intersect(subX, subY, ray, &t)) {
                float dist = t * invLength;

                if (bestDist > dist && dist >= 0.0f) {
                    bestDist = dist;
                    bestChunk = chunk;
                }
            }
        }

        if (flags & 0x30000) {
            int32_t checkType = (flags & 0x10000) && !(flags & 0x20000);

            for (CChunkLiquid* liquid = chunk->m_liquidChunkLinkList.Head(); liquid; liquid = chunk->m_liquidChunkLinkList.Next(liquid)) {
                if (checkType && !(g_liquidTypeDB.GetRecord(liquid->m_type)->m_flags & 4)) {
                    continue;
                }

                if (!liquid->TileExists(subX, subY)) {
                    continue;
                }

                uint32_t tilesY = liquid->m_tileEnd.y - liquid->m_tileBegin.y + 1;
                uint32_t vertsY = liquid->m_tileEnd.y - liquid->m_tileBegin.y + 2;
                uint32_t base = subX + tilesY * (subY - liquid->m_tileBegin.x) - liquid->m_tileBegin.y;
                uint32_t pattern[6] = { 0, vertsY, tilesY, 0, 1, vertsY };

                for (uint32_t i = 0; i < 2; i++) {
                    float t = 0.0f;
                    uint32_t tri[3] = { base + pattern[3 * i], base + pattern[3 * i + 1], base + pattern[3 * i + 2] };

                    if (NTempest::Intersect(ray, liquid->m_verts, tri, &t, nullptr, 0.0099999998f)) {
                        float dist = t * invLength;

                        if (bestDist > dist && dist >= 0.0f) {
                            bestDist = dist;
                            bestChunk = chunk;
                        }
                    }
                }
            }
        }
    }

    if (modelFlags) {
        if (flags & 0x16000AE) {
            C3Vector localStart = *start * CWorldScene::camTransportView;
            C3Vector localEnd = *end * CWorldScene::camTransportView;
            World::s_m2Scene->EndHitTest(localStart, localEnd, bestDist, 0);
        } else {
            World::s_m2Scene->EndHitTestCollisionWorld(*start, *end, bestDist);
        }
    }

    if (bestDist >= *distance) {
        return false;
    }

    *distance = bestDist;

    if (hitChunk) {
        *hitChunk = bestChunk;
    }

    return true;
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

// VERIFY: e2b4677b manual
// OFFSET: 0x007A4C10
void CMap::TestQueryAdd(const CFacet& facet, CImVector color, const C44Matrix* mat) {
    C44Matrix identity;
    uint16_t base = static_cast<uint16_t>(CMap::debugVertexArray.Count());

    if (!mat) {
        mat = &identity;
    }

    for (uint32_t i = 0; i < 3; i++) {
        CGxVertexPC* vertex = CMap::debugVertexArray.New();
        vertex->p = facet.v[i] * *mat;
        vertex->c = color;
    }

    *CMap::debugIndexArray.New() = base;
    *CMap::debugIndexArray.New() = base + 1;
    *CMap::debugIndexArray.New() = base + 2;
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

// VERIFY: 7a5c5c66 manual
// OFFSET: 0x007B47F0
static int32_t CompareAreaEntries(const void* a, const void* b) {
    const CMapAreaEntry* entryA = static_cast<const CMapAreaEntry*>(a);
    const CMapAreaEntry* entryB = static_cast<const CMapAreaEntry*>(b);

    if (entryB->m_dist > entryA->m_dist) {
        return -1;
    }

    return entryB->m_dist < entryA->m_dist;
}

// VERIFY: 9faa97d5 manual
// OFFSET: 0x007B4960
CMapArea* CMap::GetAreaAtPosition(const C2Vector* pos) {
    float x = -(pos->x - 17066.666f);
    float y = -(pos->y - 17066.666f) * 0.001875f;
    x = x * 0.001875f;

    return CMap::m_areaTable[64 * CMath::fint_(x - 0.5f) + CMath::fint_(y - 0.5f)];
}

// VERIFY: 8231832c manual
// OFFSET: 0x007B49C0
CMapChunk* CMap::GetChunkAtPosition(const C2Vector* pos) {
    float x = -(pos->x - 17066.666f);
    float y = -(pos->y - 17066.666f) * 0.03f;
    x = x * 0.03f;

    int32_t chunkY = CMath::fint_(x - 0.5f);
    int32_t chunkX = CMath::fint_(y - 0.5f);
    CMapArea* area = CMap::m_areaTable[64 * ((chunkY >> 4) & 0x3F) + ((chunkX >> 4) & 0x3F)];

    if (!area) {
        return nullptr;
    }

    return area->m_mapChunks[(chunkX & 0xF) + 16 * (chunkY & 0xF)];
}

// VERIFY: f1ab17a0 manual
// OFFSET: 0x007B4BC0
void CMap::UpdateBarriers() {
    int32_t mode = CWorldScene::s_barrier.m_mode;
    int32_t minX = CWorld::gbChunkRect.minX >> 4;
    int32_t maxX = CWorld::gbChunkRect.maxX >> 4;
    int32_t minY = CWorld::gbChunkRect.minY >> 4;
    int32_t maxY = CWorld::gbChunkRect.maxY >> 4;

    if (!mode) {
        return;
    }

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            int32_t index = 64 * y + x;

            if (!(CMap::m_areaInfo[index].m_flags & 1)) {
                continue;
            }

            CMapArea* area = CMap::m_areaTable[index];

            if (!area || area->m_asyncObject) {
                continue;
            }

            CMapArea* neighbor = CMap::m_areaTable[index - 1];

            if (!neighbor || neighbor->m_asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.b.x, area->m_bounds.t.y);
                C2Vector end(area->m_bounds.t.x, area->m_bounds.t.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index - 64];

            if (!neighbor || neighbor->m_asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.t.x, area->m_bounds.t.y);
                C2Vector end(area->m_bounds.t.x, area->m_bounds.b.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index + 1];

            if (!neighbor || neighbor->m_asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.t.x, area->m_bounds.b.y);
                C2Vector end(area->m_bounds.b.x, area->m_bounds.b.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }

            neighbor = CMap::m_areaTable[index + 64];

            if (!neighbor || neighbor->m_asyncObject || mode == 2) {
                C2Vector start(area->m_bounds.b.x, area->m_bounds.b.y);
                C2Vector end(area->m_bounds.b.x, area->m_bounds.t.y);
                CWorldScene::s_barrier.AddAxisAlignedWall(start, end, 40.0f);
                mode = CWorldScene::s_barrier.m_mode;
            }
        }
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

// VERIFY: f7ac6b15 manual
// OFFSET: 0x007B53B0
void CMap::ClearChunkSortList() {
    CMapChunk* chunk = CMap::s_mapChunkSortList.Head();

    while (chunk) {
        CMapChunk* next = CMap::s_mapChunkSortList.Next(chunk);
        chunk->m_sortListLink.Unlink();
        chunk = next;
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

// VERIFY: cd78794a manual
// OFFSET: 0x007B54A0
void CMap::ProcessDetailDoodadUpdateList() {
    CDetailDoodadInst* inst = CMap::s_detailDoodadUpdateList.Head();

    while (inst) {
        CDetailDoodadInst* next = CMap::s_detailDoodadUpdateList.Next(inst);

        if (++inst->m_age > 10) {
            if (inst->HasBatches()) {
                inst->FreeBuffers();
            }

            if (inst->m_age > 20) {
                CDetailDoodad::FreeInst(inst);
            }
        }

        inst = next;
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

// VERIFY: d6c4755a manual
// OFFSET: 0x007B5950
void CMap::PreUpdateAreas(bool a1) {
    int32_t maxY = CWorld::gbChunkRect.maxY >> 4;
    int32_t minX = CWorld::gbChunkRect.minX >> 4;
    int32_t minY = CWorld::gbChunkRect.minY >> 4;
    int32_t maxX = CWorld::gbChunkRect.maxX >> 4;
    int32_t entryCount = 0;
    CMapAreaEntry* entries = static_cast<CMapAreaEntry*>(alloca(1024 * sizeof(CMapAreaEntry)));

    CMap::ClearChunkSortList();
    CMap::ProcessChunkLiquidUpdateList();
    CMap::ProcessRenderChunkUpdateList();
    CMap::ProcessDetailDoodadUpdateList();

    int32_t waitForLoad = !(CMap::s_isStreamingMode || CMap::bPreload);
    CMapBaseObjLink* link = CMap::s_unk_ADFBEC.Head();

    while (link) {
        CMapArea* area = static_cast<CMapArea*>(link->m_owner);
        CMapBaseObjLink* next = CMap::s_unk_ADFBEC.Next(link);

        if (area->m_index.x >= minX && area->m_index.x <= maxX && area->m_index.y >= minY && area->m_index.y <= maxY) {
            C2Vector pos(World::s_currentWorldPos.x, World::s_currentWorldPos.y);
            entries[entryCount].m_dist = NTempest::DistanceSquared(area->m_bounds, pos);
            entries[entryCount].m_area = area;
            entryCount++;
        } else if (!area->m_asyncObject || !area->m_asyncObject->m_isCurrent) {
            CMap::FreeBaseObjLink(link);
            CMap::PurgeArea(area);
        }

        link = next;
    }

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            int32_t index = 64 * y + x;

            if ((CMap::m_areaInfo[index].m_flags & 1) && !CMap::m_areaTable[index]) {
                CMapArea* area = CMap::PrepareArea(x, y);
                C2Vector pos(World::s_currentWorldPos.x, World::s_currentWorldPos.y);
                entries[entryCount].m_area = area;
                entries[entryCount].m_dist = NTempest::DistanceSquared(area->m_bounds, pos);
                entryCount++;
            }
        }
    }

    if (entryCount) {
        qsort(entries, entryCount, sizeof(CMapAreaEntry), CompareAreaEntries);
    }

    int32_t lastPending = -1;
    CiRect rect;
    memset(&rect, 0, sizeof(rect));

    for (int32_t i = 0; i < entryCount; i++) {
        CMapArea* area = entries[i].m_area;

        if (!area->m_fileBuffer) {
            CMap::LoadArea(area);
        }

        if (area->m_index.x < minX || area->m_index.x > maxX || area->m_index.y < minY || area->m_index.y > maxY) {
            continue;
        }

        rect.minX = 16 * area->m_index.x;
        rect.minY = 16 * area->m_index.y;
        rect.maxX = 16 * area->m_index.x + 15;
        rect.maxY = 16 * area->m_index.y + 15;

        if (rect.minX <= CWorld::chunkRect.maxX && rect.minY <= CWorld::chunkRect.maxY && rect.maxX >= CWorld::chunkRect.minX && rect.maxY >= CWorld::chunkRect.minY && waitForLoad && area->m_asyncObject) {
            AsyncFileReadWait(area->m_asyncObject);
        }

        if (area->m_asyncObject) {
            if (CMap::s_isStreamingMode && entries[i].m_dist < 71111.109f) {
                lastPending = i;
            }
        } else {
            rect.minX = area->m_tileChunkIndex.x;
            rect.minY = area->m_tileChunkIndex.y;
            rect.maxX = area->m_tileChunkIndex.x + 15;
            rect.maxY = area->m_tileChunkIndex.y + 15;
            CMap::UpdateArea(a1, area, &rect, 0);
        }
    }

    if (CMap::s_isStreamingMode) {
        AsyncFile::EnterQueueLock();

        for (int32_t i = lastPending; i >= 0; i--) {
            CMapArea* area = entries[i].m_area;

            if (area->m_asyncObject && (entries[i].m_dist < 1111.1111f || (entries[i].m_dist < 71111.109f && !CWorldScene::FrustumCull(&area->m_bounds)))) {
                AsyncFileReadLinkObject(area->m_asyncObject, 1);
            }
        }

        AsyncFile::LeaveQueueLock();
    }

    if (a1) {
        CMap::UpdateBarriers();
    }
}

// VERIFY: 11c34df4 manual
// OFFSET: 0x007B5D00
void CMap::PrepareMapObjDef(CMapObjDef* mapObjDef, CMapObj* mapObj) {
    CAaBox bounds;
    mapObjDef->m_flags = static_cast<MapObjFlag>(mapObjDef->m_flags | 0x80);
    mapObj->GetBounds(mapObjDef->m_sphere);
    mapObjDef->m_sphere.c *= mapObjDef->m_mat;
    mapObj->GetBounds(&bounds);
    CWorldMath::TransformAABox(mapObjDef->m_mat, bounds, mapObjDef->m_bbox);
    mapObjDef->m_argbColor = *reinterpret_cast<uint32_t*>(&mapObj->m_argbColor);
    mapObjDef->m_lightsArray.SetCount(mapObj->m_ligtsCount);

    for (uint32_t i = 0; i < mapObjDef->m_lightsArray.Count(); i++) {
        mapObjDef->m_lightsArray[i] = nullptr;
    }

    CMap::CreateMapObjDefGroups(mapObj, mapObjDef);
}

// VERIFY: 61aca6d6 manual
// OFFSET: 0x007B6110
void CMap::PrepareMapObjDefs(bool a1) {
    int32_t waitLoad = 1;

    if (CMap::s_isStreamingMode || CMap::bPreload) {
        waitLoad = 0;
    }

    CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head();

    while (mapObjDef) {
        CMapObj* mapObj = mapObjDef->m_owner;
        CAaBox& defBox = mapObjDef->m_bbox;

        if (CWorld::s_objectAreaOfInterest.b.x <= defBox.t.x && CWorld::s_objectAreaOfInterest.b.y <= defBox.t.y && CWorld::s_objectAreaOfInterest.b.z <= defBox.t.z && CWorld::s_objectAreaOfInterest.t.x >= defBox.b.x && CWorld::s_objectAreaOfInterest.t.y >= defBox.b.y && CWorld::s_objectAreaOfInterest.t.z >= defBox.b.z) {
            if (waitLoad && !mapObj->m_isGroupLoaded) {
                mapObj->WaitLoad();
            }

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) && mapObj->m_isGroupLoaded) {
                CMap::PrepareMapObjDef(mapObjDef, mapObj);
            }
        }

        float dist = NTempest::DistanceSquared(defBox, World::s_currentWorldPos);

        if (dist < mapObj->m_distToCamera) {
            mapObj->m_distToCamera = dist;
        }

        for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->m_owner);
            CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 1);
            CAaBox& box = defGroup->m_bbox;

            if (CWorld::s_objectAreaOfInterest.b.x <= box.t.x && CWorld::s_objectAreaOfInterest.b.y <= box.t.y && CWorld::s_objectAreaOfInterest.b.z <= box.t.z && CWorld::s_objectAreaOfInterest.t.x >= box.b.x && CWorld::s_objectAreaOfInterest.t.y >= box.b.y && CWorld::s_objectAreaOfInterest.t.z >= box.b.z) {
                if (!(group->m_unkLoadedFlag & 1)) {
                    if (!group->m_asyncObjPtr) {
                        mapObj->ReadGroup(defGroup->m_groupNum);
                    }

                    if (waitLoad && CWorld::s_groupAreaOfInterest.b.x <= box.t.x && CWorld::s_groupAreaOfInterest.b.y <= box.t.y && CWorld::s_groupAreaOfInterest.b.z <= box.t.z && CWorld::s_groupAreaOfInterest.t.x >= box.b.x && CWorld::s_groupAreaOfInterest.t.y >= box.b.y && CWorld::s_groupAreaOfInterest.t.z >= box.b.z) {
                        mapObj->WaitLoadGroup(defGroup->m_groupNum);
                    }
                }

                group->unk_0190 = 0.0f;

                if (group->m_unkLoadedFlag & 1) {
                    if (!(defGroup->m_flags & MAPOBJ_FLAG_GROUP_INIT)) {
                        CMapObj::InitDefGroup(mapObj, group, mapObjDef, defGroup);
                    }

                    if (!(defGroup->m_flags & MAPOBJ_FLAG_REFS_CREATED)) {
                        CMapObj::CreateRefs(mapObj, group, mapObjDef, defGroup);
                        CMap::FreeBaseObjLinksInBounds(&box);
                    }
                }
            }

            float groupDist = NTempest::DistanceSquared(box, World::s_currentWorldPos);

            if (groupDist < group->m_distToCamera) {
                group->m_distToCamera = groupDist;
            }

            if (!a1 || !(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
                continue;
            }

            if (!(group->m_unkLoadedFlag & 1)) {
                CWorldScene::s_barrier.AddBarrierMapObjDefGroup(defGroup, 50.0f);
                continue;
            }

            if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
                continue;
            }

            if (CWorldScene::boundingBox.Intersects(box)) {
                CWorldScene::AddMapObjDefGroup(mapObjDef, defGroup);
            }

            for (CMapObjOccluder* occluder = group->m_occluderList.Head(); occluder; occluder = group->m_occluderList.Next(occluder)) {
                C3Vector a = occluder->m_start * mapObjDef->m_mat;
                C3Vector b = occluder->m_end * mapObjDef->m_mat;
                CWorldView::AddOccluder(a, b);
            }
        }

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
            CWorldScene::s_barrier.AddBarrier(defBox, 50.0f);
        }

        mapObjDef = CMap::mapObjDefHash.Next(mapObjDef);
    }
}

// VERIFY: 6908aa4f manual
// OFFSET: 0x007B6B00
void CMap::PrepareUpdate(bool a1) {
    if (CWorld::status.m_chunksChanged) {
        CMap::PurgeMaps();
    }

    CMap::bspRecurseCount = 0;
    CMap::mapGetFacetsCount = 0;
    CMap::oldSelectLightParm = 0;
    CMapChunkLiquidBuf::Update(FrameTime::s_tickTimeSec);
    CMapObj::PrepareUpdate();
    CMapRenderChunk::PurgeChunkBufBlocks();
    CMap::PreUpdateAreas(a1);
    CMap::PrepareMapObjDefs(a1);
    CMap::PrepareMapDoodadDefs();
    CMap::PrepareEntitys(a1);

    if (!CMap::bPreload) {
        return;
    }

    if (!CMap::s_isStreamingMode) {
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.25f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.5f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.66000003f, World::s_loadProgressParam);
        }

        CMap::PreUpdateAreas(a1);
        CMap::PrepareMapObjDefs(a1);
        CMapObj::PrepareUpdate();
        CMap::PrepareMapDoodadDefs();
        AsyncFileReadWaitAll();

        if (World::s_loadProgressCallback) {
            World::s_loadProgressCallback(0.75f, World::s_loadProgressParam);
        }

        return;
    }

    CMapArea* area = CMap::GetAreaAtPosition(reinterpret_cast<const C2Vector*>(&World::s_currentWorldPos));

    if (area && area->m_asyncObject) {
        char fileName[260];
        SFile::GetActualFileName(area->m_asyncObject->m_file, fileName, sizeof(fileName));
        AsyncFile::EnterQueueLock();
        AsyncFileReadLinkObject(area->m_asyncObject, 1);
        AsyncFile::LeaveQueueLock();

        while (area->m_asyncObject) {
            int64_t local = 0;
            int64_t total = 0;
            SFile::FileGetIsLocalAmount(fileName, local, total);
            AsyncFileReadPollHandler(nullptr, nullptr);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(static_cast<float>(local) / static_cast<float>(total) * 0.2f, World::s_loadProgressParam);
            }

            OsSleep(10);
        }
    }

    if (World::s_loadProgressCallback) {
        World::s_loadProgressCallback(0.2f, World::s_loadProgressParam);
    }

    CMap::PreUpdateAreas(a1);
    CMap::PrepareMapObjDefs(a1);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);

    CMapChunk* chunk = CMap::GetChunkAtPosition(reinterpret_cast<const C2Vector*>(&World::s_currentWorldPos));
    float progress = 0.0f;
    uint32_t doodadTotal = CMap::ComputeDoodadLoadProgress(chunk, &progress, 0);

    if (doodadTotal) {
        uint32_t remaining;

        do {
            OsSleep(10);
            AsyncFileReadPollHandler(nullptr, nullptr);
            CMap::PreUpdateAreas(a1);
            CMap::PrepareMapObjDefs(a1);
            CMapObj::PrepareUpdate();
            CMap::PrepareMapDoodadDefs();
            remaining = CMap::ComputeDoodadLoadProgress(chunk, &progress, doodadTotal);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(progress * 0.25f + 0.2f, World::s_loadProgressParam);
            }
        } while (remaining);
    }

    CMap::PreUpdateAreas(a1);
    CMap::PrepareMapObjDefs(a1);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);

    uint32_t mapObjTotal = CMap::ComputeMapObjLoadProgress(chunk, &progress, 0);
    CMapObj::PrepareUpdate();

    if (mapObjTotal) {
        uint32_t remaining;

        do {
            OsSleep(10);
            AsyncFileReadPollHandler(nullptr, nullptr);
            CMap::PreUpdateAreas(a1);
            CMap::PrepareMapObjDefs(a1);
            CMapObj::PrepareUpdate();
            CMap::PrepareMapDoodadDefs();
            remaining = CMap::ComputeMapObjLoadProgress(chunk, &progress, mapObjTotal);

            if (World::s_loadProgressCallback) {
                World::s_loadProgressCallback(static_cast<float>(mapObjTotal - remaining) * 0.30000001f / static_cast<float>(mapObjTotal) + 0.44999999f, World::s_loadProgressParam);
            }
        } while (remaining);
    }

    CMap::PreUpdateAreas(a1);
    CMap::PrepareMapObjDefs(a1);
    CMapObj::PrepareUpdate();
    CMap::PrepareMapDoodadDefs();
    AsyncFileReadPollHandler(nullptr, nullptr);
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

// VERIFY: 4426576e auto
// OFFSET: 0x007BDD70
void CMapObj::InitDefGroup(CMapObj* mapObj, CMapObjGroup* mapObjGroup, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup) {
    mapObjDefGroup->m_flags = static_cast<MapObjFlag>(mapObjDefGroup->m_flags | MAPOBJ_FLAG_GROUP_INIT);
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

// VERIFY: a7bf14e1 manual
// OFFSET: 0x007BECD0
CMapDoodadDef* CMap::CreateDoodadDef(const char* fileName, SMDoodadDef* doodadDef, C3Vector* center) {
    HASHKEY_DWORD key = { 0 };
    CMapDoodadDef* mapDoodadDef = CMap::doodadDefHash.Ptr(doodadDef->m_uniqueId, key);

    if (mapDoodadDef) {
        return mapDoodadDef;
    }

    mapDoodadDef = CMap::AllocDoodadDef();
    HASHKEY_DWORD insertKey = { 0 };
    CMap::doodadDefHash.Insert(mapDoodadDef, doodadDef->m_uniqueId, insertKey);
    CMap::doodadDefList.LinkToTail(mapDoodadDef);

    mapDoodadDef->m_position.x = -doodadDef->m_position.z;
    mapDoodadDef->m_position.y = -doodadDef->m_position.x;
    mapDoodadDef->m_position.z = doodadDef->m_position.y;
    mapDoodadDef->m_position.x = mapDoodadDef->m_position.x + center->x;
    mapDoodadDef->m_position.y = center->y + mapDoodadDef->m_position.y;
    mapDoodadDef->m_position.z = center->z + mapDoodadDef->m_position.z;

    mapDoodadDef->m_sphere.c = mapDoodadDef->m_position;
    mapDoodadDef->m_bbox.b = mapDoodadDef->m_position;
    mapDoodadDef->m_bbox.t = mapDoodadDef->m_position;
    mapDoodadDef->m_scale = doodadDef->m_scale * 0.0009765625f;
    mapDoodadDef->m_sphere.r = 0.0f;
    mapDoodadDef->m_flags = MAPOBJ_FLAG_UNPLACED;

    if (doodadDef->m_flags & 0x1) {
        mapDoodadDef->m_flags = static_cast<MapObjFlag>(MAPOBJ_FLAG_BIODOME | MAPOBJ_FLAG_UNPLACED);
    }

    mapDoodadDef->m_model = nullptr;

    float rotX = doodadDef->m_rotation.z * 0.017453292f;
    float rotY = doodadDef->m_rotation.x * 0.017453292f;
    float rotZ = 0.017453292f * doodadDef->m_rotation.y;

    mapDoodadDef->m_mat = C44Matrix();
    mapDoodadDef->m_mat.d0 = mapDoodadDef->m_position.x;
    mapDoodadDef->m_mat.d1 = mapDoodadDef->m_position.y;
    mapDoodadDef->m_mat.d2 = mapDoodadDef->m_position.z;
    mapDoodadDef->m_mat.RotateAroundZ(rotZ + 3.1415927f);
    mapDoodadDef->m_mat.RotateAroundY(rotY);
    mapDoodadDef->m_mat.RotateAroundX(rotX);
    mapDoodadDef->m_mat.Scale(mapDoodadDef->m_scale);
    mapDoodadDef->m_identity = C44Matrix();

    CM2Model* model = World::s_m2Scene->CreateModel(fileName, 0x20);
    mapDoodadDef->m_model = model;

    if (model) {
        model->m_fFlags |= 0x8000;
        model->m_worldTransform = mapDoodadDef->m_mat;
        mapDoodadDef->m_model->SetEventCallback(reinterpret_cast<void*>(CMapStaticEntity::ModelEventCallback), reinterpret_cast<uintptr_t>(mapDoodadDef));
        mapDoodadDef->m_model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        mapDoodadDef->m_model->m_lightingArg = mapDoodadDef;
        mapDoodadDef->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }

    return mapDoodadDef;
}

// VERIFY: ea13d7f6 manual
// OFFSET: 0x007BEF40
CMapDoodadDef* CMap::CreateDoodadDef(uint32_t doodadRef, SMODoodadDef* doodadDef, char* name, uint32_t key, C44Matrix* mat, uint16_t doodadSet) {
    HASHKEY_DWORD hashKey = { key };
    CMapDoodadDef* mapDoodadDef = CMap::doodadDefHash.Ptr(doodadRef, hashKey);

    if (mapDoodadDef) {
        return mapDoodadDef;
    }

    mapDoodadDef = CMap::AllocDoodadDef();
    CMap::doodadDefHash.Insert(mapDoodadDef, doodadRef, hashKey);
    CMap::doodadDefList.LinkToTail(mapDoodadDef);

    mapDoodadDef->m_position = doodadDef->m_position;
    mapDoodadDef->m_position *= *mat;
    mapDoodadDef->m_scale = doodadDef->m_scale;
    mapDoodadDef->m_sphere.c = mapDoodadDef->m_position;
    mapDoodadDef->m_sphere.r = 0.0f;
    mapDoodadDef->m_bbox.b = mapDoodadDef->m_position;
    mapDoodadDef->m_bbox.t = mapDoodadDef->m_position;
    mapDoodadDef->unk_016C = doodadSet;
    mapDoodadDef->m_flags = MAPOBJ_FLAG_UNPLACED;

    if (doodadDef->m_flags & 0x1000000) {
        mapDoodadDef->m_flags = static_cast<MapObjFlag>(MAPOBJ_FLAG_PROJ_TEX | MAPOBJ_FLAG_UNPLACED);
    }

    mapDoodadDef->m_model = nullptr;
    mapDoodadDef->m_mat = C44Matrix();
    mapDoodadDef->m_mat.Translate(doodadDef->m_position);
    mapDoodadDef->m_mat.Rotate(doodadDef->m_orientation);
    mapDoodadDef->m_mat.Scale(mapDoodadDef->m_scale);
    mapDoodadDef->m_identity = mapDoodadDef->m_mat;
    mapDoodadDef->m_mat *= *mat;

    CImVector limit;
    *reinterpret_cast<uint32_t*>(&limit) = 112;
    mapDoodadDef->AdjustLightmap(&doodadDef->m_color, &mapDoodadDef->m_m2DiffuseColor, limit, &mapDoodadDef->m_m2AmbientColor, 96);

    CM2Model* model = World::s_m2Scene->CreateModel(name, 0x20);
    mapDoodadDef->m_model = model;

    if (model) {
        model->m_fFlags |= 0x8000;
        model->m_worldTransform = mapDoodadDef->m_mat;
        mapDoodadDef->m_model->SetEventCallback(reinterpret_cast<void*>(CMapStaticEntity::ModelEventCallback), reinterpret_cast<uintptr_t>(mapDoodadDef));
        mapDoodadDef->m_model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        mapDoodadDef->m_model->m_lightingArg = mapDoodadDef;
        mapDoodadDef->m_model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    }

    return mapDoodadDef;
}

// VERIFY: fa44952a manual
// OFFSET: 0x007BF460
CMapObjDef* CMap::CreateMapObjDef(char* fileName, SMMapObjDef* mapObjectDef, C3Vector* center, bool cached) {
    CMapObjDef* mapObjDef;

    if (cached && (mapObjDef = CMap::mapObjDefHash.Ptr(mapObjectDef->m_uniqueId, CMap::s_mapObjDefHashKey)) != nullptr) {
        return mapObjDef;
    }

    mapObjDef = CMap::AllocMapObjDef();

    if (cached) {
        CMap::mapObjDefHash.Insert(mapObjDef, mapObjectDef->m_uniqueId, CMap::s_mapObjDefHashKey);
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

// VERIFY: 7ff04955 manual
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

        CMapObjDef* mapObjDef = CMap::mapObjDefHash.Ptr(def.m_uniqueId, CMap::s_mapObjDefHashKey);

        if (!mapObjDef) {
            mapObjDef = CMap::AllocMapObjDef();
            CMap::mapObjDefHash.Insert(mapObjDef, def.m_uniqueId, CMap::s_mapObjDefHashKey);

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

// VERIFY: f97c6181 manual
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
    CMap::ClearAllFootprints();

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

// VERIFY: a3468102 manual
// OFFSET: 0x007BFF70
void CMap::FreeMapObj(CMapObj* mapObj) {
    mapObj->m_asyncLoadLink.Unlink();
    mapObj->~CMapObj();
    ObjectFree(*CMap::mapObjHeap, mapObj->m_objectIndex);
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

// VERIFY: 13962642 manual
// OFFSET: 0x007C0030
void CMap::FreeMapObjGroup(CMapObjGroup* mapObjGroup) {
    mapObjGroup->m_asyncLoadLink.Unlink();
    mapObjGroup->~CMapObjGroup();
    ObjectFree(*CMap::mapObjGroupHeap, mapObjGroup->m_objectIndex);
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

// VERIFY: ef01a812 manual
// OFFSET: 0x007C0240
void CMap::FreeDoodadDef(CMapDoodadDef* doodadDef) {
    SI2::StopSound(&doodadDef->m_soundKitObject, 0, -1.0f, 1);
    doodadDef->m_lameAssLink.Unlink();
    CMap::doodadDefHash.Unlink(doodadDef);
    uint32_t* heap = CMap::doodadDefHeap;
    doodadDef->~CMapDoodadDef();
    ObjectFree(*heap, doodadDef->m_objectIndex);
}

// VERIFY: d7c82155 manual
// OFFSET: 0x007C02D0
void CMap::FreeEntity(CMapEntity* entity) {
    entity->m_lameAssLink.Unlink();
    entity->~CMapEntity();
    ObjectFree(*CMap::entityHeap, entity->m_objectIndex);
}

// VERIFY: 56cb0afe auto
// OFFSET: 0x007C0340
void CMap::FreeLight(CMapLight* light) {
    light->~CMapLight();
    ObjectFree(*CMap::lightHeap, light->m_objectIndex);
}

// VERIFY: eeef95f1 manual
// OFFSET: 0x007C0370
void CMap::FreeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    mapObjDefGroup->m_lameAssLink.Unlink();
    mapObjDefGroup->~CMapObjDefGroup();
    ObjectFree(*CMap::mapObjDefGroupHeap, mapObjDefGroup->m_objectIndex);
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

// VERIFY: eb574028 manual
// OFFSET: 0x007C0430
void CMap::FreeMapObjDef(CMapObjDef* mapObjDef) {
    mapObjDef->m_lameAssLink.Unlink();
    CMap::mapObjDefHash.Unlink(mapObjDef);
    uint32_t* heap = CMap::mapObjDefHeap;
    mapObjDef->~CMapObjDef();
    ObjectFree(*heap, mapObjDef->m_objectIndex);
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

// VERIFY: 8d425b50 manual
// OFFSET: 0x007C0610
void CMap::FreeRenderChunk(CMapRenderChunk* renderChunk) {
    renderChunk->m_renderChunkLink.Unlink();
    renderChunk->~CMapRenderChunk();
    CMap::s_mapRenderChunkFreeList.LinkToHead(renderChunk);
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

// VERIFY: 9da97e61 auto
// OFFSET: 0x007C15F0
void CMap::ClassifyStaticEntityLink(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, uint32_t faceInfo, C3Vector* pos) {
    CMapObjGroup* group = mapObjDef->m_owner->GetGroup(mapObjDefGroup->m_groupNum, 0);

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

// VERIFY: abd054b1 manual
// OFFSET: 0x007C1DC0
void CMap::LinkIntersectMapObjDefGroup(CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup, C3Vector& start, C3Vector& end, MapObjIntersectData* interiorHit, MapObjIntersectData* groundHit) {
    CMapObj* mapObj = mapObjDef->m_owner;
    CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->m_groupNum, 0);

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
        interiorHit->interior = !(mapObj->GetGroup(portalGroup->m_groupNum, 0)->m_flags & 0x8);
    }
}

// VERIFY: e5163286 manual
// OFFSET: 0x007C1F20
void CMap::FreeBaseObjLinksInBounds(CAaBox* box) {
    for (CMapEntity* entity = CMap::entityList.Head(); entity; entity = CMap::entityList.Next(entity)) {
        if (box->t.x >= entity->m_bbox.b.x && box->t.y >= entity->m_bbox.b.y && box->t.z >= entity->m_bbox.b.z && box->b.x <= entity->m_bbox.t.x && box->b.y <= entity->m_bbox.t.y && box->b.z <= entity->m_bbox.t.z) {
            CMapBaseObjLink* link = entity->m_parentLinkList.Head();

            while (link) {
                CMapBaseObjLink* next = entity->m_parentLinkList.Next(link);
                CMap::FreeBaseObjLink(link);
                link = next;
            }
        }
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

// VERIFY: 04f6c26e manual
// OFFSET: 0x007C3150
void CMap::PurgeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    for (CMapBaseObjLink* link = mapObjDefGroup->m_doodadDefLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->m_doodadDefLinkList.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);

        if (!(doodadDef->m_flags & 0x200)) {
            CMapDoodadDef::Purge(doodadDef);
        }

        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->m_entityLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->m_entityLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->m_lightLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->m_lightLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    for (CMapBaseObjLink* link = mapObjDefGroup->m_unkLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDefGroup->m_unkLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    CMap::FreeMapObjDefGroup(mapObjDefGroup);
}

// VERIFY: e0cd9ea4 manual
// OFFSET: 0x007C3250
void CMap::PurgeMapObjDef(CMapObjDef* mapObjDef) {
    if (mapObjDef->m_refCount) {
        return;
    }

    for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link;) {
        CMapBaseObjLink* next = mapObjDef->m_mapObjDefGroupLinkList.Next(link);
        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        CMap::PurgeMapObjDefGroup(mapObjDefGroup);
        link = next;
    }

    if (mapObjDef->m_defGroups.m_count == 0xFFFFFFFF) {
        mapObjDef->m_defGroups.m_dynamic.m_count = 0;
    } else {
        mapObjDef->m_defGroups.m_count = 0;
    }

    for (uint32_t i = 0; i < mapObjDef->m_lightsArray.m_count; i++) {
        if (mapObjDef->m_lightsArray.m_data[i]) {
            CMap::DestroyLight(mapObjDef->m_lightsArray.m_data[i]);
        }
    }

    mapObjDef->m_lightsArray.m_count = 0;
    CMapObj::Delete(mapObjDef->m_owner);
    mapObjDef->m_owner = nullptr;
    CMap::FreeMapObjDef(mapObjDef);

    DayNight::DNInfo* info = DayNight::GetInfo();

    if (info->m_interiorMapObjDef == mapObjDef) {
        info->m_interiorMapObjDef = nullptr;
        info->m_interiorFogBlend = 0.0f;
        info->m_interiorGroups.m_count = 0;
    }
}

// VERIFY: ad81d8da auto
// OFFSET: 0x007C3700
void CMap::PurgeArea(CMapArea* area) {
    CMap::m_areaTable[64 * area->m_index.y + area->m_index.x] = nullptr;
    area->Purge();
    CMap::FreeArea(area);
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

// VERIFY: 7e85d0ed manual
// OFFSET: 0x007D59B0
bool CMap::LocateViewerMapObjs(C3Vector& start, C3Vector& end, float dist, CMapObjDef** outDefs, uint32_t* outGroups) {
    float dists[2] = { dist, dist };
    uint32_t slot = 0;

    outDefs[0] = nullptr;
    outDefs[1] = nullptr;
    outGroups[1] = 0xFFFF;
    outGroups[0] = 0xFFFF;
    outGroups[3] = 0xFFFF;
    outGroups[2] = 0xFFFF;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef->m_flags & 0x20) {
            continue;
        }

        if (mapObjDef->m_flags & 0x400) {
            slot = 1;
        }

        CMapObj* mapObj = mapObjDef->m_owner;

        if (!mapObjDef->TestAABox(start, end) || !mapObj) {
            slot = 0;
            continue;
        }

        C3Vector localStart = start * mapObjDef->m_invMat;
        C3Vector localEnd = end * mapObjDef->m_invMat;

        if (!mapObj->TestBounds(localStart, localEnd)) {
            slot = 0;
            continue;
        }

        uint32_t exterior = 0;

        for (CMapBaseObjLink* link = mapObjDef->m_mapObjDefGroupLinkList.Head(); link; link = mapObjDef->m_mapObjDefGroupLinkList.Next(link)) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->m_owner);
            CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 0);

            if (!group || (group->m_flags & 0x410080) || !mapObj->TestGroupBounds(localStart, localEnd, defGroup->m_groupNum)) {
                continue;
            }

            C3Segment seg;
            seg.b = localStart;
            seg.t = localEnd;
            World::TriData::statusFlags = 0;
            World::TriData::nBatches = 0;
            World::TriData::faceIndexCursor = 0;
            World::TriData::indexCursor = 0;
            World::TriData::unk_CB7538 = 0;

            World::TriData triData;

            if (group->GetTris(seg, &dists[slot], 0, 0, &triData, mapObjDef)) {
                exterior = (group->m_flags >> 3) & 1;
                outDefs[slot] = mapObjDef;
                outGroups[2 * slot] = defGroup->m_groupNum;
                outGroups[2 * slot + 1] = 0xFFFF;
            }
        }

        float portalT = 1.05f;
        C3Segment seg;
        seg.b = localStart;
        seg.t = localEnd;
        uint32_t portalGroups[2];

        if (mapObj->VectorIntersectPortal(seg, &portalT, portalGroups, 0) && portalT - dists[slot] < 0.000099999997f) {
            exterior = (mapObj->GetGroupInfo(portalGroups[0])->m_flags >> 3) & 1;
            dists[slot] = portalT;
            outDefs[slot] = mapObjDef;
            outGroups[2 * slot] = portalGroups[0];

            if (mapObj->GetGroupInfo(portalGroups[1])->m_flags & 8) {
                outGroups[2 * slot + 1] = 0xFFFF;
            } else {
                outGroups[2 * slot + 1] = portalGroups[1];
            }
        }

        if (exterior) {
            outDefs[slot] = nullptr;
        }

        slot = 0;
    }

    if (outDefs[0]) {
        return true;
    }

    if (!outDefs[1]) {
        return false;
    }

    outDefs[0] = outDefs[1];
    outGroups[0] = outGroups[2];
    outGroups[1] = outGroups[3];
    outDefs[1] = nullptr;
    outGroups[2] = 0;
    outGroups[3] = 0;

    return true;
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

// VERIFY: e1002a60 manual
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

// VERIFY: c0ee5cb9 manual
// OFFSET: 0x007D9D70
void CMap::DestroyLight(CMapLight* light) {
    CMapBaseObjLink* link = light->m_parentLinkList.Head();

    while (link) {
        CMapBaseObj* ref = link->m_ref;
        CMapBaseObjLink* next = light->m_parentLinkList.Next(link);

        if (ref) {
            if (ref->m_type & 0x4) {
                static_cast<CMapChunk*>(ref)->UpdateLights();
            } else if (ref->m_type & 0x10) {
                static_cast<CMapObjDefGroup*>(ref)->UpdateLights();
            }
        }

        CMap::FreeBaseObjLink(link);
        link = next;
    }

    CMap::FreeLight(light);
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
