#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJ_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJ_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"

class C3Vector;
class C4Plane;
class CAsyncObject;
class CMapObj;
class CMapObjDef;
class CMapObjGroup;
class CShaderEffect;
struct SMODoodadDef;
struct SMODoodadSet;
struct SMOFog;
struct SMOGroupInfo;
struct SMOHeader;
struct SMOLight;
struct SMOMaterial;
struct SMOPortal;
struct SMOPortalRef;
struct SMOVisibleBlock;

class CMapObjOccluder {
    public:
    /* 0000 */ uint32_t m_memHandle;
    /* 0004 */ uint32_t unk_0004[8];
};

class CMapObjIndex {
    public:
    /* 0000 */ int32_t m_objectIndex;
};

class CMapObj : public CMapObjIndex, public TSHashObject<CMapObj, HASHKEY_NONE> {
    public:
    static TSHashTable<CMapObj, HASHKEY_NONE> mapObjHash;
    static int32_t bPoolsDirty;
    static uint32_t* occluderHeap;
    static CShaderEffect* s_classicShaders[7];
    static CShaderEffect* s_unifiedShaders[7];

    static void ClearCache(int32_t a1);
    static void Destroy();
    static void Initialize();

    /* 001C */ char m_wmoName [260];
    /* 0120 */ SMOHeader* m_header;
    /* 0124 */ char* m_textureNameList;
    /* 0128 */ char* m_groupNameList;
    /* 012C */ char* m_skybox;
    /* 0130 */ SMOGroupInfo* m_groupInfo;
    /* 0134 */ C3Vector* m_portalVertexList;
    /* 0138 */ SMOPortal* m_portalList;
    /* 013C */ SMOPortalRef* m_portalRefList;
    /* 0140 */ C3Vector* m_visBlockVertList;
    /* 0144 */ SMOVisibleBlock* m_visBlockList;
    /* 0148 */ SMOLight* m_lightList;
    /* 014C */ SMODoodadSet* m_doodadSetList;
    /* 0150 */ char* m_doodadNameList;
    /* 0154 */ SMODoodadDef* m_doodadDefList;
    /* 0158 */ SMOFog* m_fogList;
    /* 015C */ C4Plane* m_convexVolumePlanes;
    /* 0160 */ SMOMaterial* m_materialList;
    /* 0164 */ int32_t m_texturesSize;
    /* 0168 */ int32_t m_groupNameSize;
    /* 016C */ int32_t m_groupInfoCount;
    /* 0170 */ int32_t m_planeVertCount;
    /* 0174 */ int32_t m_portalsCount;
    /* 0178 */ int32_t m_portalRefCount;
    /* 017C */ int32_t m_visBlockVertCount;
    /* 0180 */ int32_t m_visBlockCount;
    /* 0184 */ int32_t m_ligtsCount;
    /* 0188 */ int32_t m_doodadSetCount;
    /* 018C */ int32_t m_doodadNameSize;
    /* 0190 */ int32_t m_doodadDefCount;
    /* 0194 */ int32_t m_fogsCount;
    /* 0198 */ int32_t m_convexVolumePlaneCount;
    /* 019C */ int32_t m_materialsCount;
    /* 01A0 */ CImVector m_argbColor;
    /* 01A4 */ int32_t unk_01A4;
    /* 01A8 */ CAaBox m_bbox;
    /* 01C0 */ float m_distToCamera;
    /* 01C4 */ int32_t unk_01C4;
    /* 01C8 */ int32_t unk_01C8;
    /* 01CC */ void* m_pWmoData;
    /* 01D0 */ int32_t m_wmoFileSize;
    /* 01D4 */ int32_t m_refCount;
    /* 01D8 */ float m_flushTimer;
    /* 01DC */ CAsyncObject* m_asyncObject;
    /* 01E0 */ int32_t m_isGroupLoaded;
    /* 01E4 */ int32_t unk_01E4;
    /* 01E8 */ TSExplicitList<CMapObjGroup> m_mapObjGroupList;
    /* 01F4 */ int32_t m_mapObjGroupCount;
    /* 01F8 */ CMapObjGroup* m_mapObjGroupArray [512];
};

struct SMOBatch {
    /* 0000 */ int16_t m_bx;
    /* 0002 */ int16_t m_by;
    /* 0004 */ int16_t m_bz;
    /* 0006 */ int16_t m_tx;
    /* 0008 */ int16_t m_ty;
    /* 000A */ int16_t m_tz;
    /* 000C */ uint32_t m_indexStart;
    /* 0010 */ uint16_t m_indexCount;
    /* 0012 */ uint16_t m_vertexStart;
    /* 0014 */ uint16_t m_vertexEnd;
    /* 0016 */ uint8_t m_flags;
    /* 0017 */ uint8_t m_texture;
};

struct SMODoodadSet {
    /* 0000 */ char m_name [20];
    /* 0014 */ uint32_t m_startIdx;
    /* 0018 */ uint32_t m_count;
    /* 001C */ uint32_t m_pad;
};

struct SMOGroupInfo {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ CAaBox m_boundingBox;
    /* 001C */ uint32_t m_nameoffset;
};

struct SMOHeader {
    /* 0000 */ uint32_t m_nTextures;
    /* 0004 */ uint32_t m_nGroups;
    /* 0008 */ uint32_t m_nPortals;
    /* 000C */ uint32_t m_nLights;
    /* 0010 */ uint32_t m_nDoodadNames;
    /* 0014 */ uint32_t m_nDoodadDefs;
    /* 0018 */ uint32_t m_nDoodadSets;
    /* 001C */ uint32_t m_ambColor;
    /* 0020 */ uint32_t m_wmoID;
    /* 0024 */ CAaBox m_boundingBox;
    /* 003C */ uint16_t m_flags;
    /* 003E */ uint16_t m_numLod;
};

struct SMOLight {
    /* 0000 */ uint8_t m_type;
    /* 0001 */ uint8_t m_atten;
    /* 0002 */ uint8_t m_pad [2];
    /* 0004 */ CImVector m_color;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ float m_intensity;
    /* 0018 */ C4Quaternion m_rotation;
    /* 0028 */ float m_attenStart;
    /* 002C */ float m_attenEnd;
};

struct SMOMaterial {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_shader;
    /* 0008 */ uint32_t m_blendMode;
    /* 000C */ uint32_t m_texture1;
    /* 0010 */ CImVector m_sidnColor;
    /* 0014 */ CImVector m_frameSidnColor;
    /* 0018 */ uint32_t m_texture2;
    /* 001C */ CImVector m_diffColor;
    /* 0020 */ uint32_t m_groundType;
    /* 0024 */ uint32_t m_texture3;
    /* 0028 */ uint32_t m_color2;
    /* 002C */ uint32_t m_flags2;
    /* 0030 */ uint32_t m_runTimeData [4];
};

struct SMOPortal {
    /* 0000 */ uint16_t m_startVertex;
    /* 0002 */ uint16_t m_count;
    /* 0004 */ C4Plane m_plane;
};

struct SMOVisibleBlock {
    /* 0000 */ uint16_t m_firstVertex;
    /* 0002 */ uint16_t m_count;
};

extern CMapObjDef* s_curMapObjDef;
extern uint32_t s_portalStamp;
extern CMapObjDef* s_stampedMapObjDef;

#endif
