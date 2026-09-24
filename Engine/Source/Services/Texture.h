#ifndef ENGINE_SOURCE_SERVICES_TEXTURE_H
#define ENGINE_SOURCE_SERVICES_TEXTURE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/Status.h"

struct CAsyncObject;

class CTexture {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ uint32_t unk_0004 [7];
    /* 0020 */ uint32_t m_uknTexFlags;
    /* 0024 */ uint32_t m_uknDword;
    /* 0028 */ uint16_t m_flags;
    /* 002A */ uint8_t m_bestMip;
    /* 002B */ uint8_t m_alphaBits;
    /* 002C */ CStatus m_status;
    /* 0040 */ CAsyncObject* m_asyncObject;
    /* 0044 */ uint32_t m_gxTex;
    /* 0048 */ uint32_t m_gxTexTarget;
    /* 004C */ uint16_t m_gxWidth;
    /* 004E */ uint16_t m_gxHeight;
    /* 0050 */ uint32_t m_gxTexFormat;
    /* 0054 */ uint32_t m_dataFormat;
    /* 0058 */ uint32_t m_gxTexFlags;
    /* 005C */ uint32_t m_atlas;
    /* 0060 */ uint32_t m_atlasBlockIndex;
    /* 0064 */ uint32_t unk_0064 [2];
    /* 006C */ char* m_filename;
};

class CTextureAtlas;

#endif
