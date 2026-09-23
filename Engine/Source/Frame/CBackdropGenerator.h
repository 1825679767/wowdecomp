#ifndef ENGINE_SOURCE_FRAME_CBACKDROPGENERATOR_H
#define ENGINE_SOURCE_FRAME_CBACKDROPGENERATOR_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Frame/Types.h"

struct CSimpleTexture;

class CBackdropGenerator {
    public:
    /* 0000 */ CSimpleTexture* m_backgroundTexture;
    /* 0004 */ uint32_t m_leftTexture;
    /* 0008 */ uint32_t m_rightTexture;
    /* 000C */ uint32_t m_topTexture;
    /* 0010 */ uint32_t m_bottomTexture;
    /* 0014 */ uint32_t m_topLeftTexture;
    /* 0018 */ uint32_t m_topRightTexture;
    /* 001C */ uint32_t m_bottomLeftTexture;
    /* 0020 */ uint32_t m_bottomRightTexture;
    /* 0024 */ RCString m_bgFile;
    /* 0030 */ RCString m_edgeFile;
    /* 003C */ uint32_t m_pieces;
    /* 0040 */ uint32_t m_tile;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ float m_edgeSize;
    /* 004C */ float m_tileSize;
    /* 0050 */ float m_insetTop;
    /* 0054 */ float m_insetBottom;
    /* 0058 */ float m_insetLeft;
    /* 005C */ float m_insetRight;
    /* 0060 */ CColor m_backgroundColor;
    /* 0064 */ CColor m_borderColor;
    /* 0068 */ uint32_t m_alphaMode;
};

#endif
