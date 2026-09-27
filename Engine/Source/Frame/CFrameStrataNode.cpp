#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CFrameStrataNode.h"

// VERIFY: 7f1b23bd manual
// OFFSET: 0x00495BD0
CFrameStrataNode::CFrameStrataNode() {
    this->m_pendingFrame = nullptr;
    this->m_batchDirty = 0;
}
