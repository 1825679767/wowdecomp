#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFONTSTRINGATTRIBUTES_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFONTSTRINGATTRIBUTES_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CSimpleFontStringAttributes {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ RCString m_font;
    /* 0010 */ float m_fontHeight;
    /* 0014 */ uint32_t m_fontFlags;
    /* 0018 */ float m_spacing;
    /* 001C */ uint32_t m_styleFlags;
    /* 0020 */ CColor m_color;
    /* 0024 */ CColor m_shadowColor;
    /* 0028 */ C2Vector m_shadowOffset;

    void SetJustifyH(uint8_t justify);
    void SetJustifyV(uint8_t justify);
};

#endif
