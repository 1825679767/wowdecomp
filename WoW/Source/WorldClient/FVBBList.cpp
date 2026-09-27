#include "FVBBList.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

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
