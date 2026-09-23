#ifndef WOW_SOURCE_LOADINGSCREEN_H
#define WOW_SOURCE_LOADINGSCREEN_H

#include <cstddef>
#include <cstdint>
#include "_Triage/CRect.h"

struct BackgroundImage {
    /* 0000 */ char* m_path;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ CRect m_rect;
};

#endif
