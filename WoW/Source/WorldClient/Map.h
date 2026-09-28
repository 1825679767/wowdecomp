#ifndef WOW_SOURCE_WORLDCLIENT_MAP_H
#define WOW_SOURCE_WORLDCLIENT_MAP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"

struct CGxBuf;
struct CGxPool;
class CGxShader;
class CMapArea;
class FVBBList;
class CMapEntity;
class CTexture;

class CGxVertexPC {
    public:
    /* 0000 */ C3Vector m_p;
    /* 000C */ CImVector m_c;
};

class CMapAreaEntry {
    public:
    /* 0000 */ CMapArea* m_area;
    /* 0004 */ float m_dist;
};

struct SMAreaInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_unused;
};

struct SMDoodadDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ uint16_t m_scale;
    /* 0022 */ uint16_t m_flags;
};

struct SMMapObjDef {
    /* 0000 */ uint32_t m_nameId;
    /* 0004 */ uint32_t m_uniqueId;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C3Vector m_rotation;
    /* 0020 */ CAaBox m_extents;
    /* 0038 */ uint16_t m_flags;
    /* 003A */ uint16_t m_doodadSet;
    /* 003C */ uint16_t m_nameSet;
    /* 003E */ uint16_t m_scale;
};

struct SMODoodadDef {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C4Quaternion m_orientation;
    /* 0020 */ float m_scale;
    /* 0024 */ CImVector m_color;
};

struct CMapFootprint {
    /* 0000 */ TSLink<CMapFootprint> m_link;
    /* 0008 */ uint8_t unk_0008[44];
};

class CMapObj;

class CMapFootprintTexture {
    public:
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ TSGrowableArray<uint32_t> unk_0004;
    /* 0014 */ uint32_t unk_0014;

    CMapFootprintTexture();
};

class CMap {
    public:
    static CGxBuf* lowDetailIndexBuf;
    static CGxPool* lowDetailIndexPool;
    static CGxShader* pixelShader_TerrainSM;
    static CGxShader* pixelShader_Terrain0_env;
    static CGxShader* vertexShader_Terrain[128];
    static CGxShader* pixelShader_Terrain3[96];
    static CGxShader* pixelShader_Terrain2[32];
    static CGxShader* pixelShader_Terrain1[32];
    static CGxShader* pixelShader_Terrain0[3];
    static int32_t uniqueId;
    static int32_t oldSelectLightParm;
    static uint32_t s_queryTag;
    static int32_t cCount;
    static uint32_t unk[4096];
    static CMapArea* m_areaTable[4096];
    static SMAreaInfo m_areaInfo[4096];
    static TSGrowableArray<uint32_t> scCollideList;
    static int32_t bActive;
    static int32_t bDungeon;
    static int32_t s_useSSE;
    static int32_t freeCounts[11];
    static float s_unk_ADF800;
    static float s_unk_CDF7D0[256];
    static float s_unk_CDFBD0[256];
    static int32_t counts[11];
    static uint32_t* lightHeap;
    static uint32_t* cacheLightHeap;
    static uint32_t* mapObjGroupHeap;
    static uint32_t* mapObjHeap;
    static uint32_t* baseObjLinkHeap;
    static uint32_t* areaHeap;
    static uint32_t* areaMedHeap;
    static uint32_t* areaLowHeap;
    static uint32_t* chunkHeap;
    static uint32_t* doodadDefHeap;
    static uint32_t* entityHeap;
    static uint32_t* mapObjDefGroupHeap;
    static uint32_t* mapObjDefHeap;
    static uint32_t* chunkLiquidHeap;
    static FVBBList s_vbbList;
    static TSExplicitList<CMapRenderChunk, offsetof(CMapRenderChunk, m_renderChunkLink)> s_mapRenderChunkFreeList;
    static TSExplicitList<CMapChunkBuf, offsetof(CMapChunkBuf, unk_0014)> s_unk_AEED80;

    static void Destroy();
    static void FootprintDestroy();
    static void FootprintInitialize();
    static void FreeMapObj(CMapObj* mapObj);
    static void Initialize();
    static void InitializeLiquidTables();
    static void InitializePCFShaders();
    static void QueryAreaId(CMapEntity* entity, uint32_t& areaId);
    static void Unload();

    /* 0000 */ uint8_t unk_0000[180];
    /* 00B4 */ uint32_t unk_00B4;
};

extern int32_t s_mapId;

class CMapAreaLow {
    public:
    /* 0000 */ uint8_t unk_0000[1];
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint8_t unk_0004;
    /* 0005 */ uint8_t unk_0005;
    /* 0006 */ uint8_t unk_0006;
    /* 0007 */ uint8_t unk_0007;
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016[4];
    /* 001A */ uint8_t unk_001A[66];
};

#endif
