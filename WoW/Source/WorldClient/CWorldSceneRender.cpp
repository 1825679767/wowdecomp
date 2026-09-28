#include "CWorldSceneRender.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

// VERIFY: 78b69e1a manual
// OFFSET: 0x007D5610
void CWorldScene::RenderCollisionDebug() {
    if (!CMap::debugVertexArray.Count()) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_PolygonOffset, 1.0f);

    C44Matrix matrix;
    matrix.Translate(C3Vector(-CWorldScene::s_activeWorldView.x, -CWorldScene::s_activeWorldView.y, -CWorldScene::s_activeWorldView.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, matrix);

    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_Culling, 0);

    CGxVertexPC* vertices = CMap::debugVertexArray.m_data;
    GxPrimVertexPtr(CMap::debugVertexArray.Count(), &vertices->p, sizeof(CGxVertexPC), nullptr, 0, &vertices->c, sizeof(CGxVertexPC), nullptr, 0, nullptr, 0);
    GxPrimIndexPtr(CMap::debugIndexArray.Count(), CMap::debugIndexArray.m_data);

    CGxBatch batch;
    batch.m_count = CMap::debugIndexArray.Count();
    batch.m_maxIndex = CMap::debugVertexArray.Count() - 1;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_minIndex = 0;
    g_theGxDevicePtr->Draw(&batch, 1);

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}
