#ifndef WOW_SOURCE_CONSOLE_CONSOLECLIENT_H
#define WOW_SOURCE_CONSOLE_CONSOLECLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
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

#endif
