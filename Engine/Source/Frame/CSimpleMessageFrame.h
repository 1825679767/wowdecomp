#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMESSAGEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleMessageFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    CSimpleMessageFrame(CSimpleFrame* parent);
};

class CSimpleMessageFrameLineNode {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
};

#endif
