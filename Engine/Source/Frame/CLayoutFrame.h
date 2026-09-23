#ifndef ENGINE_SOURCE_FRAME_CLAYOUTFRAME_H
#define ENGINE_SOURCE_FRAME_CLAYOUTFRAME_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"
#include "_Triage/CRect.h"

class CFramePoint;
class CLayoutFrame;
class CLayoutFrame_FRAMENODE;

class CLayoutFrame_FRAMENODE : public TSLink<CLayoutFrame_FRAMENODE> {
    public:
    /* 0008 */ CLayoutFrame* m_frame;
    /* 000C */ uint32_t m_dependency;
};

class CLayoutFrame {
    public:
    /* 0004 */ TSLink<void> m_resizeLink;
    /* 000C */ CFramePoint* m_framePoints [9];
    /* 0030 */ TSList<CLayoutFrame_FRAMENODE> m_resizeList;
    /* 003C */ uint32_t m_lock;
    /* 0040 */ uint32_t m_layoutFlags;
    /* 0044 */ CRect m_rect;
    /* 0054 */ float m_layoutWidth;
    /* 0058 */ float m_layoutHeight;
    /* 005C */ float m_layoutScale;
    /* 0060 */ float m_layoutDepth;
    /* 0064 */ CRect m_resizeRect;

    virtual ~CLayoutFrame();
};

#endif
