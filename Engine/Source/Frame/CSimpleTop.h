#ifndef ENGINE_SOURCE_FRAME_CSIMPLETOP_H
#define ENGINE_SOURCE_FRAME_CSIMPLETOP_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"

class CFrameStrata;
class CFrameStrataNode;
class CSimpleFrame;
struct FRAMELEVEL;

class CFrameStrata {
    public:
    /* 0000 */ int32_t m_batchDirty;
    /* 0004 */ int32_t m_levelsDirty;
    /* 0008 */ uint32_t m_topLevel;
    /* 000C */ TSFixedArray<CFrameStrataNode*> m_levels;
};

struct EVENT_DATA_KEY {
    /* 0000 */ KEY m_key;
    /* 0004 */ uint32_t m_metaKeyState;
    /* 0008 */ uint32_t m_repeat;
    /* 000C */ uint32_t m_time;
};

class CKeyEvent {
    public:
    /* 0000 */ uint32_t m_vftable;
    /* 0004 */ uint32_t m_refcnt;
    /* 0008 */ uint32_t m_id;
    /* 000C */ void* m_param;
    /* 0010 */ EVENT_DATA_KEY m_data;
};

struct FRAMELEVEL {
    /* 0000 */ CSimpleFrame* m_frame;
    /* 0004 */ uint32_t m_level;
};

template <class T>
class CSimpleSortedArray {
    public:
    /* 0000 */ TSGrowableArray<T> m_array;
    /* 0010 */ uint32_t m_count;
    /* 0014 */ uint32_t m_maxcount;
    /* 0018 */ uint32_t m_iterator;
};

class CSimpleTop : public CLayoutFrame {
    public:
    /* 0074 */ uint32_t m_screenLayer;
    /* 0078 */ CSimpleFrame* m_mouseFocus;
    /* 007C */ CSimpleFrame* m_mouseCapture;
    /* 0080 */ CSimpleFrame* m_keydownCapture [787];
    /* 0CCC */ TSExplicitList<CSimpleFrame> m_frames;
    /* 0CD8 */ TSExplicitList<CSimpleFrame> m_destroyed;
    /* 0CE4 */ CFrameStrata* m_frameStratas [9];
    /* 0D08 */ uint32_t unk_0D08;
    /* 0D0C */ uint32_t unk_0D0C;
    /* 0D10 */ uint32_t unk_0D10;
    /* 0D14 */ uint32_t unk_0D14;
    /* 0D18 */ uint32_t unk_0D18;
    /* 0D1C */ uint32_t unk_0D1C;
    /* 0D20 */ CSimpleSortedArray<FRAMELEVEL> m_eventArray [5][9];
    /* 120C */ uint32_t m_checkFocus;
    /* 1210 */ MouseEventData m_mouseEventData;
    /* 1234 */ uint32_t unk_1234;
    /* 1238 */ uint32_t m_mouseButtonHandler;
    /* 123C */ uint32_t m_mouseMoveCallback;
    /* 1240 */ uint32_t m_displaySizeChangedHandler;
    /* 1244 */ uint32_t m_focusChangedHandler;
    /* 1248 */ uint32_t m_keyDownHandler;
    /* 124C */ uint32_t unk_124C;
    /* 1250 */ uint32_t unk_1250;
    /* 1254 */ uint32_t unk_1254;
};

#endif
