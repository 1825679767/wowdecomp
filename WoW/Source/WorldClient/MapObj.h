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
#include "Engine/Source/Tempest/CWFrustum.h"

class C2Vector;
class C44Matrix;
class CFrustum;
class CMapObjDefGroup;
struct C3Segment;

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
class CAaSphere;
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

typedef void (*RENDER_CALLBACK)(uint32_t, void*);

class CMapObj : public CMapObjIndex, public TSHashObject<CMapObj, HASHKEY_NONE> {
    public:
    static TSList<CMapObj, TSGetExplicitLink<CMapObj> > s_asyncLoadQueue;
    static TSList<CMapObjGroup, TSGetExplicitLink<CMapObjGroup> > s_unk_ADFC58;
    static TSHashTable<CMapObj, HASHKEY_NONE> mapObjHash;
    static int32_t bPoolsDirty;
    static uint32_t* occluderHeap;
    static CShaderEffect* s_classicShaders[7];
    static CShaderEffect* s_unifiedShaders[7];
    static uint32_t s_unk_AEE008;

    static void ClearCache(int32_t a1);
    static void Destroy();
    static void Initialize();
    static uint32_t CreateWmoIgnoreFlags(uint32_t a1);
    static void PrepareUpdate();
    static CMapObj* Create(char* fileName);
    static void PostloadCallback(void* arg);
    static void RenderGroupCollidable(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void RenderGroupCollidableFaces(CMapObjGroup* mapObjGroup);
    static void ExteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void InteriorRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void UnifiedRender(CMapObj* mapObj, CMapObjGroup* mapObjGroup, uint32_t a3);
    static void InvokeGroupRenderCallback(CMapObj* mapObj, uint32_t groupNum);
    static void SetGroupRenderCallback(RENDER_CALLBACK callback, void* param);
    static void SetRenderModeLight();
    static void SetEmissiveColor(CImVector color);
    static void SelectWorldShaders();

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
    /* 01C4 */ TSLink<CMapObj> m_asyncLoadLink;
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

    CMapObj();
    bool Read(char* fileName);
    void ReadGroup(uint32_t index);
    void ReadGroup(uint32_t index, bool preLoad);
    void Load();
    void WaitLoad();
    void WaitLoadGroup(uint32_t index);
    void GetBounds(CAaBox* box);
    void GetGroupBounds(CAaSphere* sphere, int32_t index);
    void GetGroupBounds(CAaBox* box, int32_t index);
    int32_t GetDoodadSet(uint16_t index);
    uint32_t GetGroupFlags(int32_t index);
    SMOGroupInfo* GetGroupInfo(int32_t index);
    int32_t IsGroupLoaded(int32_t index);
    char* GetGroupName(int32_t index);
    CMapObjGroup* GetGroup(int32_t index, bool a3);
    void RenderGroup(int32_t groupIndex, C44Matrix& matrix, TSExplicitList<CWFrustum, offsetof(CWFrustum, m_link)>* frustumList);
    void CreateRefs(CMapObjGroup* mapObjGroup, CMapObjDef* mapObjDef, CMapObjDefGroup* mapObjDefGroup);
    void CreateMaterial(uint8_t texture);
    void CreateMaterials();
    bool TestBounds(CAaBox& box);
    bool TestGroupBounds(C3Vector& start, C3Vector& end, uint32_t groupNum);
    bool TestGroupBounds(C3Vector& point, uint32_t groupNum);
    bool TestGroupBounds(CAaBox& box, uint32_t groupNum, int32_t a3);
    bool GroupBoundingBoxIntersectsSphere(C3Vector& pos, uint32_t groupNum, float radius);
    bool GetTris(CFrustum* frustum, uint32_t flags, uint32_t a4, CMapObjDef* mapObjDef);
    bool VectorIntersectPortal(C3Segment& seg, float* t, int* outGroups, int useSphereTest);
    bool VectorIntersectPortals(uint32_t groupNum, C3Segment& seg, float* t, int32_t* outGroups);
    float CalcPortalFarthestDistance(SMOPortal* portal);
    bool Intersect(C3Vector& start, C3Vector& end, float* distance, uint32_t flags, uint32_t ignoreFlags, uint32_t groupNum, int32_t* hitIndex);
    SMOMaterial* GetLiquidMaterial(int32_t groupIndex);
    bool QueryLiquidStatus(uint32_t ignoreFlags, const C3Vector& pos, int32_t* liquidType, float* level);
    void QueryLiquidSounds(uint32_t groupIndex, uint32_t fromGroup, uint32_t depth, uint32_t* minDepth, const C3Vector& pos, uint32_t* results);
    uint16_t BuildLiquidVerts(CMapObjGroup* group, const C44Matrix& matrix, const CImVector* color, int32_t magma, float texCoord1U, const TSGrowableArray<uint8_t>* sharedVerts, uint32_t stride, C3Vector** position, C3Vector** normal, CImVector** vertexColor, C2Vector** texCoord0, C2Vector** texCoord1);
    void BuildLiquidIndices(CMapObjGroup* group, const TSGrowableArray<uint8_t>* sharedVerts, uint16_t** indices, uint16_t baseVertex);
    uint16_t ClipGroupGeometry(CMapObjGroup* group, const C44Matrix& matrix, const CImVector* color, int32_t magma, float texCoord1U, uint32_t stride, C3Vector** position, C3Vector** normal, CImVector** vertexColor, C2Vector** texCoord0, C2Vector** texCoord1, uint16_t** indices, uint16_t baseVertex);
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

extern uint32_t s_portalStamp;
extern CMapObjDef* s_stampedMapObjDef;

#endif

