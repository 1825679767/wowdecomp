#include "MapObj.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Storm/H/storm.h"

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

// STATIC: 0x00D1C410
int32_t CMapObj::bPoolsDirty;

// STATIC: 0x00D1C41C
CMapObjDef* s_stampedMapObjDef;

// STATIC: 0x00D1C420
CMapObjDef* s_curMapObjDef;

// STATIC: 0x00D1C424
uint32_t s_portalStamp;

// VERIFY: 2a9f3c91 manual
// OFFSET: 0x007AFEE0
void CMapObj::Initialize() {
    s_portalStamp = 0;
    s_curMapObjDef = nullptr;
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
