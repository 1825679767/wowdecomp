#ifndef ENGINE_SOURCE_GXU_GXUFONTSTRING_H
#define ENGINE_SOURCE_GXU_GXUFONTSTRING_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/cimvector.h"
#include "Storm/H/stpl.h"

class CGxString;

class CGxString : public TSLinkedNode<CGxString> {
    public:
    /* 0008 */ TSLink<CGxString> m_fontStringLink;
    /* 0010 */ TSLink<CGxString> m_batchedStringLink;
    /* 0018 */ float m_requestedFontHeight;
    /* 001C */ float m_currentFontHeight;
    /* 0020 */ C3Vector m_position;
    /* 002C */ CImVector m_fontColor;
    /* 0030 */ CImVector m_shadowColor;
    /* 0034 */ C2Vector m_shadowOffset;
    /* 003C */ float m_blockWidth;
    /* 0040 */ float m_blockHeight;
    /* 0044 */ void* m_currentFace;
    /* 0048 */ char* m_text;
    /* 004C */ int32_t m_textLen;
    /* 0050 */ int32_t m_vertJust;
    /* 0054 */ int32_t m_horzJust;
    /* 0058 */ float m_spacing;
    /* 005C */ uint32_t m_flags;
    /* 0060 */ uint32_t m_texturePagesUsed;
    /* 0064 */ int32_t m_textureEvicted;
    /* 0068 */ uint32_t m_lastGradientStart;
    /* 006C */ uint32_t m_lastGradientLength;
    /* 0070 */ C3Vector m_viewTranslation;
    /* 007C */ float m_scale;
    /* 0080 */ uint32_t m_hyperlinkInfo [4];
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t m_dwordA0;
    /* 00A4 */ uint32_t m_dwordA4;
    /* 00A8 */ uint32_t m_dwordA8;
    /* 00AC */ uint32_t m_dwordAC;
    /* 00B0 */ int32_t m_intB0;
    /* 00B4 */ void* m_textLines [8];
    /* 00D4 */ int32_t m_intD4;
};

struct VERT {
    /* 0000 */ C3Vector m_vc;
    /* 000C */ C2Vector m_tc;
};

#endif
