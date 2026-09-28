#include "MapObjGroup.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00AEEE58
VBBList CMapObjGroup::vertexVBList;

// STATIC: 0x00AEEE80
VBBList CMapObjGroup::indexVBList;

// STATIC: 0x00AEEEA8
TSExplicitList<CMapObjGroup::VertArray, offsetof(CMapObjGroup::VertArray, m_link)> CMapObjGroup::s_unk_AEEEA8;

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

// VERIFY: 6dcd01f3 manual
// OFFSET: 0x007CB870
void VBBList::Destroy() {
    VBBList_Block* block = this->unk_001C.Head();

    while (!this->unk_001C.IsEnd(block)) {
        VBBList_Block* next = this->unk_001C.RawNext(block);
        CGxBuf* buffer = block->m_buffer;

        if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buffer);
        }

        block->m_buffer = nullptr;

        if (!this->m_singlePool) {
            if (block->m_pool) {
                g_theGxDevicePtr->PoolDestroy(block->m_pool);
            }

            block->m_pool = nullptr;
        }

        this->unk_001C.DeleteNode(block);
        block = next;
    }

    if (this->m_singlePool) {
        block = this->unk_0010.Head();

        while (!this->unk_0010.IsEnd(block)) {
            VBBList_Block* next = this->unk_0010.RawNext(block);
            CGxBuf* buffer = block->m_buffer;

            if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
                g_theGxDevicePtr->BufDestroy(buffer);
            }

            block->m_buffer = nullptr;
            this->unk_0010.DeleteNode(block);
            block = next;
        }

        if (this->m_singlePool && this->m_pool) {
            g_theGxDevicePtr->PoolDestroy(this->m_pool);
        }
    }
}

// VERIFY: 431d1d34 auto
// OFFSET: 0x007CB9D0
void CMapObjGroup::Destroy() {
    CMapObjGroup::vertexVBList.Destroy();
    CMapObjGroup::indexVBList.Destroy();
    CMapObjGroup::s_unk_AEEEA8.Clear();
}
