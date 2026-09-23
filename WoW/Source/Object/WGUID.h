#ifndef WOW_SOURCE_OBJECT_WGUID_H
#define WOW_SOURCE_OBJECT_WGUID_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct WGUID {
    /* 0000 */ uint32_t m_guidLow;
    /* 0004 */ uint32_t m_guidHigh;
};

#endif
