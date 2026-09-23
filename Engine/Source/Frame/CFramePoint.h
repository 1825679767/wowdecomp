#ifndef ENGINE_SOURCE_FRAME_CFRAMEPOINT_H
#define ENGINE_SOURCE_FRAME_CFRAMEPOINT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct CLayoutFrame;

class CFramePoint {
    public:
    /* 0000 */ float m_x;
    /* 0004 */ float m_y;
    /* 0008 */ CLayoutFrame* m_relativeFrame;
    /* 000C */ uint32_t m_relativePoint;
};

#endif
