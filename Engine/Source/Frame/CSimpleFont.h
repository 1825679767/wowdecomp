#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONT_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFONT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFontStringAttributes.h"
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CSimpleFontable;

class CSimpleFont : public FrameScript_Object {
    public:
    /* 0014 */ CSimpleFontable m_baseSimpleFontable;
    /* 0028 */ CSimpleFontStringAttributes m_attributes;
    /* 0058 */ char* m_name;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ TSList<CSimpleFontable> m_fontableList;
};

#endif
