#include "FVBBList.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"

// VERIFY: 59fe6fba manual
// OFFSET: 0x007D58B0
void FVBBList::Initialize(EGxPoolTarget target, EGxPoolUsage usage, uint32_t itemSize, uint32_t itemCount, uint32_t blockCount) {
    this->m_blockSize = itemSize * itemCount;
    this->m_pool = g_theGxDevicePtr->PoolCreate(target, usage, blockCount * this->m_blockSize, GxPoolHintBit_Unk2, target ? "FVBBList_idx" : "FVBBList_vtx");

    for (uint32_t i = 0; i < blockCount; i++) {
        Block* block = this->m_blocks.NewNode(2, 0, 0);
        block->m_buf = g_theGxDevicePtr->BufCreate(this->m_pool, itemSize, itemCount, i * this->m_blockSize);
    }
}

// VERIFY: 7f2e6e9c manual
// OFFSET: 0x007D5D50
void FVBBList::Destroy() {
    Block* block = this->m_blocks.Head();

    while (!this->m_blocks.IsEnd(block)) {
        Block* next = this->m_blocks.RawNext(block);
        CGxBuf* buf = block->m_buf;

        if (buf && buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }

        this->m_blocks.DeleteNode(block);
        block = next;
    }

    if (this->m_pool) {
        g_theGxDevicePtr->PoolDestroy(this->m_pool);
    }
}
