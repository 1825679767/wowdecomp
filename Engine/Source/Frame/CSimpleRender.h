#ifndef ENGINE_SOURCE_FRAME_CSIMPLERENDER_H
#define ENGINE_SOURCE_FRAME_CSIMPLERENDER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CSimpleTexture;
struct FONTHASHOBJ;
struct TEXTBLOCK;

class CSimpleFontString : public CSimpleRegion {
    public:
    /* 00D4 */ CSimpleFontable m_baseSimpleFontable;
    /* 00E8 */ FONTHASHOBJ* m_fontHandle;
    /* 00EC */ float m_fontHeight;
    /* 00F0 */ uint16_t m_textMaxSize;
    /* 00F2 */ uint16_t m_textCurSize;
    /* 00F4 */ char* m_text;
    /* 00F8 */ float m_spacing;
    /* 00FC */ TEXTBLOCK* m_string;
    /* 0100 */ float m_textWidth;
    /* 0104 */ float m_textHeight;
    /* 0108 */ CColor m_shadowColor;
    /* 010C */ C2Vector m_shadowOffset;
    /* 0114 */ C2Vector m_justificationOffset;
    /* 011C */ int16_t m_alphaGradientStart;
    /* 011E */ int16_t m_alphaGradientLength;
    /* 0120 */ uint32_t m_styleFlags;
    /* 0124 */ uint32_t m_settableStyleFlags;
    /* 0128 */ uint32_t m_maxLines;
    /* 012C */ uint32_t unk_012C;
    /* 0130 */ uint32_t unk_0130;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ TSList<CSimpleTexture> m_textures;
};

#endif
