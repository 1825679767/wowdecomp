#ifndef ENGINE_SOURCE_MODEL2_M2SCENE_H
#define ENGINE_SOURCE_MODEL2_M2SCENE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

struct C3Vector;
class CM2Cache;
class CM2Light;
class CM2Model;
struct CMapBaseObj;
struct CShaderEffect;
struct M2Batch;
struct M2Element;

struct M2Element {
    /* 0000 */ int32_t m_type;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ float m_alpha;
    /* 0010 */ float unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ int32_t m_index;
    /* 001C */ int32_t m_priorityPlane;
    /* 0020 */ M2Batch* m_batch;
    /* 0024 */ void* m_skinSection;
    /* 0028 */ CShaderEffect* m_effect;
    /* 002C */ uint32_t m_vertexPermute;
    /* 0030 */ uint32_t m_pixelPermute;
};

class CM2Scene {
    public:
    /* 0000 */ int32_t m_refCount;
    /* 0004 */ CM2Cache* m_cache;
    /* 0008 */ CM2Model* m_modelList;
    /* 000C */ uint32_t m_time;
    /* 0010 */ uint32_t m_timeDelta;
    /* 0014 */ uint32_t m_frameStamp;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t m_flags;
    /* 0020 */ CM2Light* m_lightList;
    /* 0024 */ CM2Light** m_lightGrid;
    /* 0028 */ CM2Model* m_animateList;
    /* 002C */ CM2Model* m_drawList;
    /* 0030 */ CM2Model* m_particleList;
    /* 0034 */ TSGrowableArray<M2Element> m_elements;
    /* 0044 */ TSGrowableArray<uint8_t> m_doodadElements;
    /* 0054 */ TSGrowableArray<uint8_t> m_passElements [3];
    /* 0084 */ C44Matrix m_view;
    /* 00C4 */ C44Matrix m_viewInv;
    /* 0104 */ uint32_t m_projectTextureCallback;
    /* 0108 */ uint32_t m_projectTextureParam;
    /* 010C */ uint32_t m_projectPositionCallback;
    /* 0110 */ uint32_t m_projectPositionParam;
    /* 0114 */ CM2Model* m_hitTestList;
    /* 0118 */ M2HitRec* m_hitRecs;
    /* 011C */ uint32_t* m_hitOrder;
    /* 0120 */ uint32_t m_hitCapacity;
    /* 0124 */ C3Vector* m_hitVerts;
    /* 0128 */ uint32_t m_hitVertCapacity;
    /* 012C */ M2HitRec m_lastHit;
    /* 013C */ CMapBaseObj* m_lastHitOwner;
    /* 0140 */ uint32_t m_liquidTypeId;
    /* 0144 */ uint32_t m_passMask;
};

#endif
