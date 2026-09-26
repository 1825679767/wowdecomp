#ifndef ENGINE_SOURCE_FRAME_TYPES_H
#define ENGINE_SOURCE_FRAME_TYPES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

class CScriptRegionAnimGroupNode;
class CSimpleFrame;
struct SIMPLEFRAMENODE;

enum ANCHORPOINT {
    ANCHOR_TOP_LEFT = 0,
    ANCHOR_TOP = 1,
    ANCHOR_TOP_RIGHT = 2,
    ANCHOR_LEFT = 3,
    ANCHOR_CENTER = 4,
    ANCHOR_RIGHT = 5,
    ANCHOR_BOTTOM_LEFT = 6,
    ANCHOR_BOTTOM = 7,
    ANCHOR_BOTTOM_RIGHT = 8,
};

class CColor {
    public:
    /* 0000 */ uint8_t m_b;
    /* 0001 */ uint8_t m_g;
    /* 0002 */ uint8_t m_r;
    /* 0003 */ uint8_t m_a;
};

class CScriptRegionAnimGroupNode {
    public:
    /* 0000 */ CScriptRegionAnimGroupNode* m_prev;
    /* 0004 */ CScriptRegionAnimGroupNode* m_next;
    /* 0008 */ void* m_group;
};

enum CSimpleEventType {
    SIMPLE_EVENT_CHAR = 0,
    SIMPLE_EVENT_KEY = 1,
    SIMPLE_EVENT_MOUSE = 2,
    SIMPLE_EVENT_MOUSEWHEEL = 3,
    SIMPLE_EVENT_4 = 4,
    NUM_SIMPLE_EVENTS = 5,
};

struct SIMPLEFRAMENODE {
    /* 0000 */ TSLink<SIMPLEFRAMENODE> m_link;
    /* 0008 */ CSimpleFrame* m_frame;
};

#endif
