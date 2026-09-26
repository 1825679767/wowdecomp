#ifndef ENGINE_SOURCE_GX_CGXFONT_H
#define ENGINE_SOURCE_GX_CGXFONT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Gxu/GxuFontMiscClasses.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CGxString;
struct CHARCODEDESC;
struct HASHKEY_NONE;
struct KERNINGHASHKEY;
struct KERNNODE;

struct KERNINGHASHKEY {
    /* 0000 */ uint32_t m_code;
};

struct KERNNODE {
    /* 0000 */ TSHashObject<KERNNODE, KERNINGHASHKEY> m_baseHashObject;
    /* 0018 */ uint32_t m_flags;
    /* 001C */ float m_proporportionalSpacing;
    /* 0020 */ float m_fixedWidthSpacing;
};

class CGxFont {
    public:
    /* 0000 */ TSLink<CGxString> m_baseLink;
    /* 0008 */ TSList<CGxString> m_strings;
    /* 0014 */ TSHashTable<CHARCODEDESC, HASHKEY_NONE> m_activeCharacters;
    /* 003C */ TSHashTable<KERNNODE, KERNINGHASHKEY> m_kernInfo;
    /* 0064 */ TSList<CHARCODEDESC> m_activeCharacterCache;
    /* 0070 */ uint32_t m_fontFaceHandle;
    /* 0074 */ char m_fontName [260];
    /* 0178 */ uint32_t m_cellHeight;
    /* 017C */ uint32_t m_baseline;
    /* 0180 */ uint32_t m_fontFlags;
    /* 0184 */ float m_fontHeight;
    /* 0188 */ float m_pixelsPerUnit;
    /* 018C */ TEXTURECACHE m_textureCache [8];
    /* 024C */ uint32_t m_pixelSize;
};

#endif
