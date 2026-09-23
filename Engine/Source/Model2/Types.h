#ifndef ENGINE_SOURCE_MODEL2_TYPES_H
#define ENGINE_SOURCE_MODEL2_TYPES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class CM2ModelCall;

class CM2ModelCall {
    public:
    /* 0000 */ uint32_t m_type;
    /* 0004 */ CM2ModelCall* m_next;
    /* 0008 */ uint32_t m_time;
    /* 000C */ uint32_t m_args [8];
};

#endif
