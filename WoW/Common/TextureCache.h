#ifndef WOW_COMMON_TEXTURECACHE_H
#define WOW_COMMON_TEXTURECACHE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Storm/H/stpl.h"
#include "WoW/Common/ComponentCore/Types.h"

struct BLPHeader;
class CACHEENTRY;
class CAsyncObject;
class CGxFont;
struct HASHKEY_NONE;
class TEXTURECACHEROW;

struct BLPHeader {
    /* 0000 */ uint32_t m_magic;
    /* 0004 */ uint32_t m_formatVersion;
    /* 0008 */ uint8_t m_colorEncoding;
    /* 0009 */ uint8_t m_alphaSize;
    /* 000A */ uint8_t m_preferredFormat;
    /* 000B */ uint8_t m_hasMips;
    /* 000C */ uint32_t m_width;
    /* 0010 */ uint32_t m_height;
    /* 0014 */ uint32_t m_mipOffsets [16];
    /* 0054 */ uint32_t m_mipSizes [16];
    /* 0094 */ uint8_t m_extended[1024];
};

class CACHEENTRY : public TSHashObject<CACHEENTRY, HASHKEY_NONE> {
    public:
    /* 0018 */ CAsyncObject* m_asyncObject;
    /* 001C */ TCTEXTUREINFO m_info;
    /* 0024 */ char m_fileName [128];
    /* 00A4 */ uint32_t m_refCount;
    /* 00A8 */ uint32_t m_memHandle;
    /* 00AC */ BLPHeader* m_data;
    /* 00B0 */ uint32_t m_flags;
};

class TEXTURECACHE {
    public:
    /* 0000 */ uint32_t m_texture;
    /* 0004 */ CGxFont* m_theFace;
    /* 0008 */ int8_t m_anyDirtyGlyphs;
    /* 0009 */ int8_t m_pad [3];
    /* 000C */ TSBaseArray<TEXTURECACHEROW> m_textureRows;
};

#endif
