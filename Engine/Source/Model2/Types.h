#ifndef ENGINE_SOURCE_MODEL2_TYPES_H
#define ENGINE_SOURCE_MODEL2_TYPES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

enum M2LIGHTTYPE {
    M2LIGHT_DIRECT = 0,
    M2LIGHT_POINT = 1,
};

class CM2ModelCall;

class CM2ModelCall {
    public:
    /* 0000 */ uint32_t m_type;
    /* 0004 */ CM2ModelCall* m_next;
    /* 0008 */ uint32_t m_time;
    /* 000C */ uint32_t m_args [8];
};

#endif
