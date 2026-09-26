#ifndef ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H
#define ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleStatusBar : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    CSimpleStatusBar(CSimpleFrame* parent);
};

#endif
