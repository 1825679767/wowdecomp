#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONTABLE_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFONTABLE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleFont;
class CSimpleFontable;

class CSimpleFontable {
    public:
    /* 0004 */ CSimpleFont* m_fontObject;
    /* 0008 */ uint32_t m_fontableFlags;
    /* 000C */ TSLink<CSimpleFontable> m_fontableLink;

    virtual ~CSimpleFontable();
};

#endif
