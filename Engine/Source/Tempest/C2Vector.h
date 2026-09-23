#ifndef ENGINE_SOURCE_TEMPEST_C2VECTOR_H
#define ENGINE_SOURCE_TEMPEST_C2VECTOR_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class C2Vector {
    public:
    /* 0000 */ float m_x;
    /* 0004 */ float m_y;
};

struct MouseEventData {
    /* 0000 */ uint32_t m_mode;
    /* 0004 */ uint32_t m_button;
    /* 0008 */ uint32_t m_buttonState;
    /* 000C */ uint32_t m_metaKeyState;
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ C2Vector m_mousePosition;
    /* 001C */ uint32_t m_wheelAmount;
    /* 0020 */ uint32_t m_eventTime;
};

#endif
