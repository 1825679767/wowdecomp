#ifndef ENGINE_SOURCE_GXU_GXUFONTMISCCLASSES_H
#define ENGINE_SOURCE_GXU_GXUFONTMISCCLASSES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

class CColor;
class CHARCODEDESC;
struct HASHKEY_NONE;
struct VERT;

class BATCHEDRENDERFONTDESC;

class GLYPHBITMAPDATA {
    public:
    /* 0000 */ uint32_t m_data;
    /* 0004 */ uint32_t m_dataSize;
    /* 0008 */ uint32_t m_glyphWidth;
    /* 000C */ uint32_t m_glyphHeight;
    /* 0010 */ int32_t m_glyphCellWidth;
    /* 0014 */ float m_glyphAdvance;
    /* 0018 */ float m_glyphBearing;
    /* 001C */ uint32_t m_glyphPitch;
    /* 0020 */ int32_t m_yOffset;
    /* 0024 */ int32_t m_yStart;
    /* 0028 */ CRect m_textureCoords;
};

class CHARCODEDESC {
    public:
    /* 0000 */ TSHashObject<CHARCODEDESC, HASHKEY_NONE> m_hashObject;
    /* 0018 */ TSLink<CHARCODEDESC> m_textureRowLink;
    /* 0020 */ TSLink<CHARCODEDESC> m_fontGlyphLink;
    /* 0028 */ uint32_t m_textureNumber;
    /* 002C */ uint32_t m_rowNumber;
    /* 0030 */ uint32_t m_glyphStartPixel;
    /* 0034 */ uint32_t m_glyphEndPixel;
    /* 0038 */ GLYPHBITMAPDATA m_bitmapData;
};

class TEXTLINETEXTURE {
    public:
    /* 0000 */ TSGrowableArray<VERT> m_verts;
    /* 0010 */ TSGrowableArray<CColor> m_colors;
};

class TEXTURECACHEROW {
    public:
    /* 0000 */ uint32_t m_widestFreeSlot;
    /* 0004 */ TSList<CHARCODEDESC> m_glyphlist;
};

#endif
