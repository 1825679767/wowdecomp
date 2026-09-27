#include "MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00AEEE58
VBBList CMapObjGroup::vertexVBList;

// STATIC: 0x00AEEE80
VBBList CMapObjGroup::indexVBList;

// VERIFY: 03d37c83 auto
// OFFSET: 0x007CB990
void CMapObjGroup::Initialize() {
    CMapObjGroup::vertexVBList.m_singlePool = 0;
    CMapObjGroup::vertexVBList.m_target = GxPoolTarget_Vertex;
    CMapObjGroup::vertexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::unk_CD7658);
    CMapObjGroup::vertexVBList.m_pool = nullptr;
    CMapObjGroup::indexVBList.m_singlePool = 0;
    CMapObjGroup::indexVBList.m_target = GxPoolTarget_Index;
    CMapObjGroup::indexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::unk_CD7658);
    CMapObjGroup::indexVBList.m_pool = nullptr;
}
