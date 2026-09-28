#include "MapObj.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/WorldClient/Map.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include <cmath>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Gxu/GxuLight.h"

// STATIC: 0x00AEE008
uint32_t CMapObj::s_unk_AEE008 = 2;

// STATIC: 0x00ADFF50
static int32_t unk_ADFF50 = 2048;

// STATIC: 0x00D1BEE8
static TSGrowableArray<C3Vector> unk_D1BEE8;

// STATIC: 0x00D1C3D4
CShaderEffect* CMapObj::s_unifiedShaders[7];

// STATIC: 0x00D1C3F0
CShaderEffect* CMapObj::s_classicShaders[7];

// STATIC: 0x00D1C40C
uint32_t* CMapObj::occluderHeap;

// STATIC: 0x00D1C454
TSHashTable<CMapObj, HASHKEY_NONE> CMapObj::mapObjHash;

// STATIC: 0x00D1C410
int32_t CMapObj::bPoolsDirty;

// STATIC: 0x00D1C41C
CMapObjDef* s_stampedMapObjDef;

// STATIC: 0x00D1C424
uint32_t s_portalStamp;

// STATIC: 0x00D1C428
static HASHKEY_NONE s_mapObjHashKey;

// VERIFY: bd88db75 auto
// OFFSET: 0x007A7B00
static void __stdcall WriteLiquidVertex(CMapObjGroup* group, const C44Matrix& matrix, const C3Vector& vertex, const SMOLiquidVert* liquidVert, const CImVector* color, int32_t magma, float texCoord1U, int32_t* unused, uint32_t stride, C3Vector** position, C3Vector** normal, CImVector** vertexColor, C2Vector** texCoord0, C2Vector** texCoord1) {
    DepthCoordTable* depthTable = CMap::GetDepthCoordTable(group->m_liquidType);

    if (*position) {
        **position = vertex * matrix;
        *position = reinterpret_cast<C3Vector*>(reinterpret_cast<uint8_t*>(*position) + stride);
    }

    if (*normal) {
        (*normal)->x = 0.0f;
        (*normal)->y = 0.0f;
        (*normal)->z = 1.0f;
        *normal = reinterpret_cast<C3Vector*>(reinterpret_cast<uint8_t*>(*normal) + stride);
    }

    if (*vertexColor) {
        CImVector value;

        if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
            value.b = color->r;
            value.g = color->g;
            value.r = color->b;
            value.a = color->a;
        } else {
            value = *color;
        }

        **vertexColor = value;
        *vertexColor = reinterpret_cast<CImVector*>(reinterpret_cast<uint8_t*>(*vertexColor) + stride);
    }

    if (*texCoord0) {
        if (magma) {
            (*texCoord0)->x = liquidVert->m_magmaVert.m_s * 0.00390625f;
            (*texCoord0)->y = liquidVert->m_magmaVert.m_t * 0.00390625f;
        } else {
            C44Matrix rotation = matrix;
            rotation.d0 = 0.0f;
            rotation.d1 = 0.0f;
            rotation.d2 = 0.0f;
            rotation.d3 = 1.0f;

            C3Vector local;
            local.x = vertex.x - group->m_liquidCorner.x;
            local.y = vertex.y - group->m_liquidCorner.y;
            local.z = vertex.z - group->m_liquidCorner.z;

            C2Vector uv(local * rotation);
            (*texCoord0)->x = uv.x * 0.24f;
            (*texCoord0)->y = 0.24f * uv.y;
        }

        *texCoord0 = reinterpret_cast<C2Vector*>(reinterpret_cast<uint8_t*>(*texCoord0) + stride);
    }

    if (*texCoord1) {
        float depth = depthTable ? (*depthTable)[liquidVert->m_waterVert.m_flow1] : 0.0f;
        (*texCoord1)->x = texCoord1U;
        (*texCoord1)->y = depth;
        *texCoord1 = reinterpret_cast<C2Vector*>(reinterpret_cast<uint8_t*>(*texCoord1) + stride);
    }
}

// VERIFY: f7f5970e auto
// OFFSET: 0x007AE1A0
void CMapObj::ReadGroup(uint32_t index) {
    this->ReadGroup(index, false);
}

// VERIFY: d92cbc0c auto
// OFFSET: 0x007AE1C0
void CMapObj::WaitLoad() {
    while (this->m_asyncObject) {
        AsyncFileReadWait(this->m_asyncObject);
    }
}

// VERIFY: c6f5dc77 manual
// OFFSET: 0x007AE4C0
int32_t CMapObj::IsGroupLoaded(int32_t index) {
    if (this->m_isGroupLoaded) {
        return this->m_mapObjGroupArray[index]->m_unkLoadedFlag & 1;
    }

    return 0;
}

// VERIFY: 8c25cdca manual
// OFFSET: 0x007AE7E0
bool CMapObj::TestBounds(CAaBox& box) {
    return this->m_isGroupLoaded && this->m_bbox.Intersects(box);
}

// VERIFY: 0d13eb33 auto
// OFFSET: 0x007AEAB0
void CMapObj::WaitLoadGroup(uint32_t index) {
    CMapObjGroup* group = this->m_mapObjGroupArray[index];

    while (group->m_asyncObjPtr) {
        AsyncFileReadWait(group->m_asyncObjPtr);
    }
}

// VERIFY: baf76f3a manual
// OFFSET: 0x007AFEE0
void CMapObj::Initialize() {
    s_portalStamp = 0;
    CWorldScene::s_curMapObjDef = nullptr;
    s_stampedMapObjDef = nullptr;
    unk_ADFF50 = 2048;
    unk_D1BEE8.Reserve(1024, 0);
    unk_D1BEE8.m_chunk = 1024;

    CMapObj::s_classicShaders[0] = CShaderEffectManager::GetEffect("MapObjDiffuse");
    CMapObj::s_classicShaders[1] = CShaderEffectManager::GetEffect("MapObjSpecular");
    CMapObj::s_classicShaders[2] = CShaderEffectManager::GetEffect("MapObjMetal");
    CMapObj::s_classicShaders[3] = CShaderEffectManager::GetEffect("MapObjEnv");
    CMapObj::s_classicShaders[4] = CShaderEffectManager::GetEffect("MapObjOpaque");
    CMapObj::s_classicShaders[5] = CShaderEffectManager::GetEffect("MapObjEnvMetal");
    CMapObj::s_classicShaders[6] = nullptr;

    CMapObj::s_unifiedShaders[0] = CShaderEffectManager::GetEffect("MapObjUDiffuse");
    CMapObj::s_unifiedShaders[1] = CShaderEffectManager::GetEffect("MapObjUSpecular");
    CMapObj::s_unifiedShaders[2] = CShaderEffectManager::GetEffect("MapObjUMetal");
    CMapObj::s_unifiedShaders[3] = CShaderEffectManager::GetEffect("MapObjUEnv");
    CMapObj::s_unifiedShaders[4] = CShaderEffectManager::GetEffect("MapObjUOpaque");
    CMapObj::s_unifiedShaders[5] = CShaderEffectManager::GetEffect("MapObjUEnvMetal");
    CMapObj::s_unifiedShaders[6] = CShaderEffectManager::GetEffect("MapObjUComposite");

    CMapObj::bPoolsDirty = 1;
    CMapObj::occluderHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(sizeof(CMapObjOccluder), 128, "MAPOBJOCC", true));
}

// VERIFY: 81a97e96 auto
// OFFSET: 0x007B01C0
void CMapObj::Destroy() {
    CMapObj::ClearCache(1);
    CMapObjGroup::Destroy();

    for (uint32_t i = 0; i < sizeof(CMapObj::s_classicShaders) / sizeof(CMapObj::s_classicShaders[0]); i++) {
        CMapObj::s_classicShaders[i] = nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CMapObj::s_unifiedShaders) / sizeof(CMapObj::s_unifiedShaders[0]); i++) {
        CMapObj::s_unifiedShaders[i] = nullptr;
    }

    CShaderEffectManager::DestroyAllEffects();

    if (CMapObj::occluderHeap) {
        SMemFree(CMapObj::occluderHeap, "delete", SLOG_FUNCTION, 0);
        CMapObj::occluderHeap = nullptr;
    }
}

// VERIFY: e6f8b5e3 manual
// OFFSET: 0x007B0040
void CMapObj::ClearCache(int32_t a1) {
    CMapObj* mapObj = CMapObj::mapObjHash.m_fulllist.Head();

    while (mapObj) {
        CMapObj* next = CMapObj::mapObjHash.Next(mapObj);

        if (!mapObj->m_refCount) {
            CMapObj::mapObjHash.Unlink(mapObj);
            CMap::FreeMapObj(mapObj);
        }

        mapObj = next;
    }
}







// OFFSET: 0x007A8800
void CMapObj::SetRenderModeLight() {
    CGxLight light;
    light.m_flags |= 0x1;

    float d = 1.0f / sqrtf(3.0f);
    light.m_dir = { d, d, d };
    light.m_ambientColor = { 0.33f, 0.33f, 0.33f };
    light.m_dirColor = { 0.75f, 0.75f, 0.75f };
    light.m_specularColor = { 0.0f, 0.0f, 0.0f };
    light.m_constantAttenuation = 0.0f;
    light.m_linearAttenuation = 0.0f;
    light.m_quadraticAttenuation = 0.0f;

    C3Vector origin = { 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->LightSet(0, light, origin);
    g_theGxDevicePtr->LightEnable(0, 1);

    for (int i = 1; i < 4; i++)
        g_theGxDevicePtr->LightEnable(i, 0);

    GxRsSet(GxRs_Lighting, 1);
}






// OFFSET: 0x007AE140
uint32_t CMapObj::CreateWmoIgnoreFlags(uint32_t a1) {
    uint32_t result = 0xEE;
    if ((a1 & 0x80u) != 0)
        return 0x2CA;
    if ((a1 & 0x10) != 0)
        result = 0xC6;
    if ((a1 & 0x20) != 0)
        result &= 0xDBu;
    if ((a1 & 0x40) == 0)
        result &= ~2u;
    if ((a1 & 0x4000) == 0)
        result &= ~0x40u;
    if ((a1 & 0x1000000) != 0) {
        if ((a1 & 0x40000000) != 0)
            return 0xFFFFFCDB;
        else
            return result | 0x100;
    }
    return result;
}



























// VERIFY: 81665f7f manual
// OFFSET: 0x0079BBF0
void CMapObj::QueryLiquidSounds(uint32_t groupIndex, uint32_t fromGroup, uint32_t depth, uint32_t* minDepth, const C3Vector& pos, uint32_t* results) {
    if (depth > CMapObj::s_unk_AEE008) {
        return;
    }

    if (!this->GetGroupInfo(groupIndex)->m_boundingBox.ContainsPoint(pos)) {
        return;
    }

    CMapObjGroup* group = this->GetGroup(groupIndex, false);

    if (!group) {
        return;
    }

    if ((group->m_flags & 0x8) && depth < *minDepth) {
        *minDepth = depth;
    }

    group->QueryLiquidSounds(pos, results);

    for (uint32_t i = 0; i < static_cast<uint32_t>(group->m_portalCount); i++) {
        uint32_t neighbor = this->m_portalRefList[group->m_portalStart + i].m_groupIndex;

        if (neighbor != 0xFFFF && neighbor != fromGroup) {
            this->QueryLiquidSounds(neighbor, groupIndex, depth + 1, minDepth, pos, results);
        }
    }
}

// VERIFY: 3402cfca auto
// OFFSET: 0x007A6D70
SMOMaterial* CMapObj::GetLiquidMaterial(int32_t groupIndex) {
    CMapObjGroup* group = this->GetGroup(groupIndex, false);

    if (!group) {
        return nullptr;
    }

    return &this->m_materialList[static_cast<uint16_t>(group->m_luquidMaterialId)];
}

// VERIFY: 64db3118 auto
// OFFSET: 0x007AEB90
bool CMapObj::QueryLiquidStatus(uint32_t ignoreFlags, const C3Vector& pos, int32_t* liquidType, float* level) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_groupInfoCount); i++) {
        if (!this->m_isGroupLoaded) {
            continue;
        }

        SMOGroupInfo* info = &this->m_groupInfo[i];

        if (!info || (ignoreFlags & info->m_flags) || !info->m_boundingBox.ContainsPoint(pos)) {
            continue;
        }

        CMapObjGroup* group = this->m_mapObjGroupArray[i];

        if ((group->m_unkLoadedFlag & 0x1) && group->QueryLiquid(pos, liquidType, level)) {
            return true;
        }
    }

    return false;
}

// VERIFY: bad0310c manual
// OFFSET: 0x007A7920
void CMapObj::BuildLiquidIndices(CMapObjGroup* group, const TSGrowableArray<uint8_t>* sharedVerts, uint16_t** indices, uint16_t baseVertex) {
    int32_t rowStride = group->m_liquidVerts.x + sharedVerts[0].m_count;
    uint16_t lastIndex = 0;
    int32_t inStrip = 0;
    uint32_t sharedRow = 0;
    int32_t outRow = 0;

    for (int32_t y = 0; y < group->m_liquidTiles.y; y++) {
        int32_t duplicate = 0;

        if (sharedRow < sharedVerts[1].m_count && sharedVerts[1].m_data[sharedRow] == y) {
            duplicate = 1;
            sharedRow++;
        }

        int32_t nextRow = baseVertex + rowStride;

        for (int32_t r = duplicate + 1; r; r--) {
            uint16_t top = outRow * rowStride + baseVertex;
            uint16_t bottom = nextRow + top;
            uint32_t sharedCol = 0;

            for (int32_t x = 0; x < group->m_liquidTiles.x; x++) {
                uint8_t flags = group->m_liquidTileList[group->m_liquidTiles.x * y + x].m_flags;
                int32_t visible = (flags & 0xF) != 0xF && (flags & 0x80) ? 1 : 0;
                int32_t duplicateCol = 0;

                if (sharedCol < sharedVerts[0].m_count && sharedVerts[0].m_data[sharedCol] == x) {
                    duplicateCol = 1;
                    sharedCol++;
                }

                uint16_t bottomNext = bottom + 1;

                for (int32_t c = duplicateCol + 1; c; c--) {
                    if (visible) {
                        if (!inStrip) {
                            inStrip = 1;
                            *(*indices)++ = top;
                            *(*indices)++ = top;
                            *(*indices)++ = bottom;
                        }

                        *(*indices)++ = top + 1;
                        **indices = bottomNext;
                        lastIndex = bottomNext;
                        (*indices)++;
                    } else if (inStrip) {
                        **indices = lastIndex;
                        inStrip = 0;
                        (*indices)++;
                    }

                    top++;
                    bottom++;
                    bottomNext++;
                }
            }

            if (inStrip) {
                *(*indices)++ = lastIndex;
                inStrip = 0;
            }

            outRow++;
        }
    }
}

// VERIFY: 5e3b42d7 manual
// OFFSET: 0x007A7CC0
uint16_t CMapObj::BuildLiquidVerts(CMapObjGroup* group, const C44Matrix& matrix, const CImVector* color, int32_t magma, float texCoord1U, const TSGrowableArray<uint8_t>* sharedVerts, uint32_t stride, C3Vector** position, C3Vector** normal, CImVector** vertexColor, C2Vector** texCoord0, C2Vector** texCoord1) {
    float y = group->m_liquidCorner.y;
    uint32_t sharedRow = 0;

    for (int32_t row = 0; row < group->m_liquidVerts.y; row++) {
        int32_t duplicateRow = 0;

        if (sharedRow < sharedVerts[1].m_count && sharedVerts[1].m_data[sharedRow] == row) {
            duplicateRow = 1;
            sharedRow++;
        }

        for (int32_t r = duplicateRow + 1; r; r--) {
            float x = group->m_liquidCorner.x;
            uint32_t sharedCol = 0;

            for (int32_t col = 0; col < group->m_liquidVerts.x; col++) {
                const SMOLiquidVert* liquidVert = &group->m_liquidVertexList[group->m_liquidVerts.x * row + col];

                C3Vector vertex;
                vertex.x = x;
                vertex.y = y;
                vertex.z = liquidVert->m_waterVert.m_height;

                int32_t duplicateCol = 0;

                if (sharedCol < sharedVerts[0].m_count && sharedVerts[0].m_data[sharedCol] == col) {
                    duplicateCol = 1;
                    sharedCol++;
                }

                int32_t unused;

                for (int32_t c = duplicateCol + 1; c; c--) {
                    WriteLiquidVertex(group, matrix, vertex, liquidVert, color, magma, texCoord1U, &unused, stride, position, normal, vertexColor, texCoord0, texCoord1);
                }

                x += 4.1666665f;
            }
        }

        y += 4.1666665f;
    }

    return (group->m_liquidVerts.x + sharedVerts[0].m_count) * (sharedVerts[1].m_count + group->m_liquidVerts.y);
}

// VERIFY: 3da651a7 auto
// OFFSET: 0x007B0CC0
CMapObj* CMapObj::Create(char* fileName) {
    uint32_t hashval = SStrHash(fileName, 0, 0);
    CMapObj* mapObj = CMapObj::mapObjHash.Ptr(hashval, s_mapObjHashKey);

    if (mapObj) {
        mapObj->m_refCount++;
        return mapObj;
    }

    mapObj = CMap::AllocMapObj();

    if (!mapObj->Read(fileName)) {
        DebugLog("CMapObj::Create(): mapObj->Read(\"%s\") failed", fileName);
    }

    CMapObj::mapObjHash.Insert(mapObj, hashval, s_mapObjHashKey);
    mapObj->m_refCount = 1;

    return mapObj;
}
