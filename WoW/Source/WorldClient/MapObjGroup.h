#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/AaBsp.h"
#include "Engine/Source/Tempest/C2iVector.h"

struct BspQuery_Segment;
class C2Vector;
class C3Vector;
class C44Matrix;
class CAaBsp;
class CAsyncObject;
struct CGxBuf;
struct CGxPool;
class CImVector;
class CMapObj;
class CMapObjDef;
class CMapObjGroup;
struct SMOBatch;
struct SMOLTile;
struct SMOLiquidVert;
struct SMOMaterial;
struct SMOPoly;
class VBBList_Block;

struct BspQuery {
    /* 0000 */ int32_t* m_overflowFlags;
    /* 0004 */ SMOPoly* m_faces;
    /* 0008 */ C3Vector* m_vertexList;
    /* 000C */ uint16_t* m_indices;
};

struct BspQuery_Segment : public BspQuery {
    /* 0010 */ float* m_hitT;
    /* 0014 */ float m_origHitT;
    /* 0018 */ C3Segment m_seg;
    /* 0030 */ C3Ray m_ray;
    /* 0048 */ float m_oosegMag;
    /* 004C */ float m_maxT;
    /* 0050 */ SMOMaterial* m_materials;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ uint16_t m_faceIgnoreFlags;
    /* 005A */ uint8_t unk_005A[2];
};

class CAaBsp_Query_BspQuery_Segment {
    public:
    /* 0000 */ const CAaBsp* m_aaBsp;
    /* 0004 */ BspQuery_Segment* m_f;
};

class CGxVertexPNC2T2 {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
    /* 0018 */ CImVector m_color [2];
    /* 0020 */ C2Vector m_texture [2];
};

class CGxVertexPNCT {
    public:
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_normal;
    /* 0018 */ CImVector m_color;
    /* 001C */ C2Vector m_texture;
};

class CMapObjGroup {
    public:
    /* 0000 */ int32_t m_objectIndex;
    /* 0004 */ VBBList_Block* m_vertsBlock;
    /* 0008 */ VBBList_Block* m_transparencyVertsBlock;
    /* 000C */ VBBList_Block* m_indicesBlock;
    /* 0010 */ VBBList_Block* m_liquidVertsBlock;
    /* 0014 */ VBBList_Block* m_liquidIndicesBlock;
    /* 0018 */ float m_timer;
    /* 001C */ void* unk_001C;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int16_t unk_002C [2];
    /* 0030 */ int32_t m_flags;
    /* 0034 */ CAaBox m_bbox;
    /* 004C */ float m_distToCamera;
    /* 0050 */ int32_t m_portalStart;
    /* 0054 */ int32_t m_portalCount;
    /* 0058 */ int32_t m_fogs;
    /* 005C */ uint16_t m_transparencyBatchesCount;
    /* 005E */ uint16_t m_intBatchCount;
    /* 0060 */ uint16_t m_extBatchCount;
    /* 0062 */ uint8_t unk_0062[2];
    /* 0064 */ CAaBsp m_cAaBspNodePtr1;
    /* 00CC */ int32_t m_unkFlags;
    /* 00D0 */ int32_t unk_00D0;
    /* 00D4 */ int32_t m_minimapTag;
    /* 00D8 */ char* m_groupName;
    /* 00DC */ SMOPoly* m_polyList;
    /* 00E0 */ uint16_t* m_indices;
    /* 00E4 */ int32_t unk_00E4;
    /* 00E8 */ C3Vector* m_vertexList;
    /* 00EC */ C3Vector* m_normalList;
    /* 00F0 */ C2Vector* m_textureVertexList;
    /* 00F4 */ int32_t unk_00F4;
    /* 00F8 */ SMOBatch* m_batchList;
    /* 00FC */ int32_t unk_00FC;
    /* 0100 */ int32_t unk_0100;
    /* 0104 */ uint16_t* m_doodadRefList;
    /* 0108 */ CImVector* m_colorVertexList;
    /* 010C */ CImVector* m_colorVertexListExtra;
    /* 0110 */ int32_t unk_0110;
    /* 0114 */ C2iVector m_liquidVerts;
    /* 011C */ C2iVector m_liquidTiles;
    /* 0124 */ C3Vector m_liquidCorner;
    /* 0130 */ int32_t m_luquidMaterialId;
    /* 0134 */ SMOLiquidVert* m_liquidVertexList;
    /* 0138 */ SMOLTile* m_liquidTileList;
    /* 013C */ float m_liquidHeight;
    /* 0140 */ int32_t unk_0140;
    /* 0144 */ int32_t m_liquidType;
    /* 0148 */ int32_t m_unkFlag;
    /* 014C */ int32_t unk_014C;
    /* 0150 */ int32_t m_polyListSize;
    /* 0154 */ int32_t m_indicesCount;
    /* 0158 */ int32_t unk_0158;
    /* 015C */ int32_t m_vertexListCount;
    /* 0160 */ int32_t m_normalListCount;
    /* 0164 */ int32_t m_textureVertexListCount;
    /* 0168 */ int32_t unk_0168;
    /* 016C */ int32_t m_batchListCount;
    /* 0170 */ int32_t unk_0170;
    /* 0174 */ int32_t m_doodadRefListCount;
    /* 0178 */ int32_t m_colorVertexListSize;
    /* 017C */ int32_t m_colorVertexListExtraSize;
    /* 0180 */ int32_t m_wmoGroupId;
    /* 0184 */ void* m_filePtr;
    /* 0188 */ int32_t m_fileSize;
    /* 018C */ CMapObj* m_parent;
    /* 0190 */ int32_t unk_0190;
    /* 0194 */ CAsyncObject* m_asyncObjPtr;
    /* 0198 */ int32_t m_unkLoadedFlag;
    /* 019C */ int32_t m_unkIndexMin1;
    /* 01A0 */ int32_t m_unkIndexMax1;
    /* 01A4 */ uint16_t m_unkIndexMin2;
    /* 01A6 */ uint16_t m_unkIndexMax2;
    /* 01A8 */ int32_t m_tSExplicitListMLinkoffset;
    /* 01AC */ void* m_tSExplicitListPtr;
    /* 01B0 */ void* m_tSExplicitListPtr2;
    /* 01B4 */ CMapObjGroup* m_perv;
    /* 01B8 */ CMapObjGroup* m_next;
};

struct SMOGroupHeader {
    /* 0000 */ uint32_t m_groupName;
    /* 0004 */ uint32_t m_descriptiveGroupName;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ CAaBox m_boundingBox;
    /* 0024 */ uint16_t m_portalStart;
    /* 0026 */ uint16_t m_portalCount;
    /* 0028 */ uint16_t m_transBatchCount;
    /* 002A */ uint16_t m_intBatchCount;
    /* 002C */ uint16_t m_extBatchCount;
    /* 002E */ uint16_t m_paddingOrBatchTypeD;
    /* 0030 */ uint8_t m_fogIds [4];
    /* 0034 */ uint32_t m_groupLiquid;
    /* 0038 */ uint32_t m_uniqueID;
    /* 003C */ uint32_t unk_003C;
};

struct SMOLTile {
    /* 0000 */ uint8_t m_flags;
};

struct SMOLiquidVert {
    /* 0000 */ uint8_t unk_0000[8];
};

struct SMOMVert {
    /* 0000 */ int16_t m_s;
    /* 0002 */ int16_t m_t;
    /* 0004 */ float m_height;
};

struct SMOWVert {
    /* 0000 */ uint8_t m_flow1;
    /* 0001 */ uint8_t m_flow2;
    /* 0002 */ uint8_t m_flow1Pct;
    /* 0003 */ uint8_t m_filler;
    /* 0004 */ float m_height;
};

class VBBList_Block {
    public:
    /* 0000 */ TSLink<VBBList_Block> m_link;
    /* 0008 */ EGxPoolUsage m_poolUsage;
    /* 000C */ uint32_t m_offset;
    /* 0010 */ uint32_t m_capacity;
    /* 0014 */ CGxPool* m_pool;
    /* 0018 */ CGxBuf* m_buffer;
    /* 001C */ VBBList_Block* m_listHead;
};

class VBBList {
    public:
    /* 0000 */ int32_t m_singlePool;
    /* 0004 */ EGxPoolTarget m_target;
    /* 0008 */ EGxPoolUsage m_usage;
    /* 000C */ CGxPool* m_pool;
    /* 0010 */ TSList<VBBList_Block> unk_0010;
    /* 001C */ TSList<VBBList_Block> unk_001C;
};

namespace World {
namespace TriData {
struct Batch {
    /* 0000 */ C44Matrix* m_matrix;
    /* 0004 */ C3Vector* m_vertexList;
    /* 0008 */ C3Vector* m_normalList;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint16_t* m_indices;
    /* 0014 */ uint16_t* m_faceIndices;
    /* 0018 */ uint16_t m_indexCount;
    /* 001A */ uint16_t m_faceCount;
    /* 001C */ uint16_t m_minVertexIndex;
    /* 001E */ uint16_t m_maxVertexIndex;
    /* 0020 */ CMapObjDef* m_def;
};
}
}

class CAaBsp_Query_Segment {
    public:
    /* 0000 */ uint32_t unk_0000;
};

class WMOGroup {
    public:
    /* 0000 */ uint8_t unk_0000[284];
    /* 011C */ uint32_t unk_011C;
    /* 0120 */ uint8_t unk_0120[24];
    /* 0138 */ uint32_t unk_0138;
};

#endif
