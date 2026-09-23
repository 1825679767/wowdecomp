#ifndef WOW_SOURCE_WORLDCLIENT_MAPCHUNK_H
#define WOW_SOURCE_WORLDCLIENT_MAPCHUNK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/caabox.h"
#include "Engine/Source/Tempest/caasphere.h"
#include "Engine/Source/Tempest/cimvector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "_Triage/C2iVector.h"

class CChunkLiquid;
class CMapChunk;
struct CMapChunkDoodadDefLink;
class CMapChunkMapObjDefLink;
struct CMapObjDef;
struct CMapRenderChunk;
class CWSoundEmitter;
struct SMChunk;
struct SMLayer;
struct SMLiquidChunk;

class CGxVertexPN {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
};

class CGxVertexPNC {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
    /* 0018 */ CImVector m_color;
};

class CMapChunkMapObjDefLink {
    public:
    /* 0000 */ uint32_t m_objectIndex;
    /* 0004 */ CMapObjDef* m_owner;
    /* 0008 */ CMapChunk* m_ref;
    /* 000C */ TSLink<CMapChunkMapObjDefLink> m_refLink;
    /* 0014 */ TSLink<CMapChunkMapObjDefLink> m_ownerLink;
};

class CMapChunk : public CMapBaseObj {
    public:
    /* 0024 */ C2iVector m_aIndex;
    /* 002C */ C2iVector m_sOffset;
    /* 0034 */ C2iVector m_cOffset;
    /* 003C */ CAaSphere m_sphere;
    /* 004C */ CAaBox m_bbox;
    /* 0064 */ C3Vector m_bottomRight;
    /* 0070 */ C3Vector m_topLeft;
    /* 007C */ C3Vector m_topLeftCoords;
    /* 0088 */ float m_distToCamera;
    /* 008C */ CAaBox m_bbox2;
    /* 00A4 */ void* m_detailDoodadInst;
    /* 00A8 */ CMapRenderChunk* m_renderChunk;
    /* 00AC */ int32_t m_bLoaded;
    /* 00B0 */ int32_t m_areaId;
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;
    /* 00BC */ TSLink<CMapChunk> m_sortListLink;
    /* 00C4 */ TSExplicitList<CMapChunkDoodadDefLink> m_doodadDefLinkList;
    /* 00D0 */ TSExplicitList<CMapChunkMapObjDefLink> m_mapObjDefLinkList;
    /* 00DC */ int32_t m_tSExplicitListMLinkoffsetDc;
    /* 00E0 */ void* m_tSExplicitListPtrE0;
    /* 00E4 */ void* m_tSExplicitListPtr2E4;
    /* 00E8 */ int32_t m_tSExplicitListMLinkoffsetE8;
    /* 00EC */ void* m_tSExplicitListPtrEc;
    /* 00F0 */ void* m_tSExplicitListPtr2F0;
    /* 00F4 */ int32_t m_tSExplicitListMLinkoffsetF4;
    /* 00F8 */ void* m_tSExplicitListPtrF8;
    /* 00FC */ void* m_tSExplicitListPtr2Fc;
    /* 0100 */ TSExplicitList<CChunkLiquid> m_liquidChunkLinkList;
    /* 010C */ uint8_t* m_chunkInfoBeginPtr;
    /* 0110 */ SMChunk* m_header;
    /* 0114 */ uint8_t* m_lowQualityTexMap;
    /* 0118 */ uint8_t* m_predTexture;
    /* 011C */ float* m_height;
    /* 0120 */ uint32_t* m_vertexShading;
    /* 0124 */ int8_t* m_normals;
    /* 0128 */ uint8_t* m_shadowMap;
    /* 012C */ SMLayer* m_layers;
    /* 0130 */ uint8_t* m_additionalShadowmap;
    /* 0134 */ uint8_t* m_mcrfPtr;
    /* 0138 */ SMLiquidChunk* m_liquid;
    /* 013C */ CWSoundEmitter* m_soundEmitters;
    /* 0140 */ int32_t unk_0140;
    /* 0144 */ int32_t unk_0144;
    /* 0148 */ int32_t unk_0148;
    /* 014C */ int32_t unk_014C;
    /* 0150 */ int32_t unk_0150;
    /* 0154 */ int32_t unk_0154;
};

class CWSoundEmitter {
    public:
    /* 0000 */ uint32_t m_entryId;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ C3Vector m_size;
};

struct SLTiles {
    /* 0000 */ char m_tiles [8][8];
};

struct SLVert {
    /* 0000 */ uint8_t unk_0000[8];
};

struct SMChunk {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C2iVector m_index;
    /* 000C */ uint32_t m_nLayers;
    /* 0010 */ uint32_t m_nDoodadRefs;
    /* 0014 */ uint32_t m_ofsHeight;
    /* 0018 */ uint32_t m_ofsNormal;
    /* 001C */ uint32_t m_ofsLayer;
    /* 0020 */ uint32_t m_ofsRefs;
    /* 0024 */ uint32_t m_ofsAlpha;
    /* 0028 */ uint32_t m_sizeAlpha;
    /* 002C */ uint32_t m_ofsShadow;
    /* 0030 */ uint32_t m_sizeShadow;
    /* 0034 */ uint32_t m_areaid;
    /* 0038 */ uint32_t m_nMapObjRefs;
    /* 003C */ uint16_t m_holes;
    /* 003E */ uint16_t unk_003E;
    /* 0040 */ uint8_t m_lowQualityTextureMap [16];
    /* 0050 */ uint32_t m_predTex;
    /* 0054 */ uint32_t m_nEffectDoodad;
    /* 0058 */ uint32_t m_ofsSndEmitters;
    /* 005C */ uint32_t m_nSndEmitters;
    /* 0060 */ uint32_t m_ofsLiquid;
    /* 0064 */ uint32_t m_sizeLiquid;
    /* 0068 */ C3Vector m_position;
    /* 0074 */ uint32_t m_ofsMCCV;
    /* 0078 */ uint32_t m_unused1;
    /* 007C */ uint32_t m_unused2;
};

struct SWFlowv {
    /* 0000 */ CAaSphere m_sphere;
    /* 0010 */ C3Vector m_dir;
    /* 001C */ float m_velocity;
    /* 0020 */ float m_amplitude;
    /* 0024 */ float m_frequency;
};

struct SMLiquidChunk {
    /* 0000 */ float m_minHeight;
    /* 0004 */ float m_maxHeight;
    /* 0008 */ SLVert m_verts [81];
    /* 0290 */ SLTiles m_tiles;
    /* 02D0 */ uint32_t m_nFlowvs;
    /* 02D4 */ SWFlowv m_flowvs [2];
};

struct SMVert {
    /* 0000 */ uint16_t m_s;
    /* 0002 */ uint16_t m_t;
    /* 0004 */ float m_height;
};

struct SOVert {
    /* 0000 */ char m_depth;
    /* 0001 */ char m_foam;
    /* 0002 */ char m_wet;
    /* 0003 */ char m_filler;
};

struct SWVert {
    /* 0000 */ char m_depth;
    /* 0001 */ char m_flow0Pct;
    /* 0002 */ char m_flow1Pct;
    /* 0003 */ char m_filler;
    /* 0004 */ float m_height;
};

#endif
