#ifndef WOW_SOURCE_CONSOLE_CONSOLECLIENT_H
#define WOW_SOURCE_CONSOLE_CONSOLECLIENT_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"
#include "WoW/Source/Console/Types.h"

class CGxString;
class CONSOLELINE;

class CONSOLELINE : public TSLinkedNode<CONSOLELINE> {
    public:
    /* 0008 */ uint8_t* m_buffer;
    /* 000C */ uint32_t m_chars;
    /* 0010 */ uint32_t m_charsalloc;
    /* 0014 */ uint32_t m_inputpos;
    /* 0018 */ uint32_t m_inputstart;
    /* 001C */ COLOR_T m_colorType;
    /* 0020 */ CGxString* m_fontPointer;
};

struct EVENT_DATA_MOUSE {
    /* 0000 */ MOUSEMODE m_mode;
    /* 0004 */ MOUSEBUTTON m_button;
    /* 0008 */ uint32_t m_buttonState;
    /* 000C */ uint32_t m_metaKeyState;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ float m_x;
    /* 0018 */ float m_y;
    /* 001C */ int32_t m_wheelDistance;
    /* 0020 */ uint32_t m_time;
};

#endif
