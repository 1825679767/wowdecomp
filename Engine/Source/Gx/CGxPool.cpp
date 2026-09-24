#include <cstdint>
#include "Engine/Source/Gx/CGxPool.h"

// VERIFY: 3a3418b6 manual
// OFFSET: 0x00688230
void CGxPool::Invalidate() {
    for (CGxBuf* buf = this->m_bufList.Head(); buf; buf = this->m_bufList.Next(buf)) {
        buf->unk_001C = 0;
    }
}

// VERIFY: 9189f656 manual
// OFFSET: 0x00688260
void CGxPool::Discard() {
    for (CGxBuf* buf = this->m_bufList.Head(); buf; buf = this->m_bufList.Next(buf)) {
        buf->unk_001C = 0;
    }

    this->unk_001C = 0;
}
