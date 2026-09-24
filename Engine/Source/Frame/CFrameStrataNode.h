#ifndef ENGINE_SOURCE_FRAME_CFRAMESTRATANODE_H
#define ENGINE_SOURCE_FRAME_CFRAMESTRATANODE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CRenderBatch;
struct CSimpleFrame;

class CFrameStrataNode {
    public:
    /* 0000 */ TSExplicitList<CSimpleFrame> m_pendingFrames;
    /* 000C */ TSExplicitList<CSimpleFrame> m_frames;
    /* 0018 */ CSimpleFrame* m_pendingFrame;
    /* 001C */ CRenderBatch m_batches [5];
    /* 010C */ int32_t m_batchDirty;
    /* 0110 */ TSExplicitList<CRenderBatch> m_renderList;
};

#endif
