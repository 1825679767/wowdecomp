#include "ShadowQuery.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cmath>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/CPortalView.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D25304
float CShadowQuery::s_cutoffHeight;

// STATIC: 0x00D25320
ShadowQueryBuffers CShadowQuery::s_unk_D25320[3];

// VERIFY: a0192ad6 auto
// OFFSET: 0x007BD3A0
void CShadowQuery::Initialize() {
    CShadowCache::SetMatrixCallback(CShadowQuery::MatrixCallback);
    CShadowCache::SetFrustumCallback(CShadowQuery::FrustumCallback);
    CShadowCache::SetQueryCallback(CShadowQuery::QueryCallback);
    CShadowCache::SetRenderCallback(CShadowQuery::RenderCallback);

    g_shadowCull = CVar::Register("shadowCull", "enable shadow frustum culling", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    g_shadowScissor = CVar::Register("shadowScissor", "enable scissoring when rendering shadowmaps", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    g_shadowInstancing = CVar::Register("shadowInstancing", "enable instancing when rendering shadowmaps", 0, "1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);

    CShadowCache::Init();
}

// OFFSET: 0x007BA7D0
void CShadowQuery::FreeBuffers() {
    for (ShadowQueryBuffers* buffers = CShadowQuery::s_unk_D25320; buffers < CShadowQuery::s_unk_D25320 + sizeof(CShadowQuery::s_unk_D25320) / sizeof(CShadowQuery::s_unk_D25320[0]); buffers++) {
        if (buffers->unk_0004) {
            SMemFree(buffers->unk_0004, nullptr, 0, 0);
        }

        if (buffers->unk_0008) {
            SMemFree(buffers->unk_0008, nullptr, 0, 0);
        }

        delete[] buffers->unk_000C;
        delete[] buffers->unk_0018;
    }
}

// VERIFY: 11a5d7a4 manual
// OFFSET: 0x007BB3E0
static void GetViewerPosition(C3Vector* position) {
    *position = DayNight::GetInfo()->m_cameraPos;
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        *position = object->GetPosition();
    }
}

// VERIFY: aa0465cc auto
// OFFSET: 0x007BB570
void CShadowQuery::Update() {
    C3Vector direction = CMap::s_mapLight->m_light.m_dir;
    int32_t useCullFrustum = CWorldScene::frustumPortalView.m_maxViewDepth >= 0.0f || CWorldScene::s_unk_CD8778;
    float z = direction.z * 5.0f;

    if (z < -1.2f) {
        z = -1.2f;
    }

    float inv = 1.0f / std::sqrt(direction.y * direction.y + direction.x * direction.x + z * z);
    direction.x = direction.x * inv;
    direction.y = direction.y * inv;
    direction.z = z * inv;
    CShadowCache::PreUpdate(direction);
    C3Vector viewer(0.0f, 0.0f, 0.0f);
    GetViewerPosition(&viewer);
    CShadowCache::SetDebugParams(1.0f, 0);
    CShadowCache::Update(viewer, useCullFrustum);
    CShadowCache::PostUpdate();
}
