#ifndef ENGINE_SOURCE_BLPFILE_BLP_H
#define ENGINE_SOURCE_BLPFILE_BLP_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/BLPFile/Types.h"
#include "WoW/Common/TextureCache.h"

struct MipBits;

class CBLPFile {
    public:
    /* 0000 */ MipBits* m_images;
    /* 0004 */ BLPHeader m_header;
    /* 0498 */ void* m_inMemoryImage;
    /* 049C */ int32_t m_inMemoryNeedsFree;
    /* 04A0 */ uint32_t m_numLevels;
    /* 04A4 */ uint32_t m_quality;
    /* 04A8 */ uint32_t m_colorMapping;
    /* 04AC */ MipMapAlgorithm m_mipMapAlgorithm;
    /* 04B0 */ uint8_t* m_lockDecompMem;
};

#endif
