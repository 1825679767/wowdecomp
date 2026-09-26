#ifndef ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_H
#define ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleColorSelect : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    CSimpleColorSelect(CSimpleFrame* parent);
};

#endif
