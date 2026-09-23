#ifndef ENGINE_SOURCE_GXU_EMBEDDEDPARSEINFO_H
#define ENGINE_SOURCE_GXU_EMBEDDEDPARSEINFO_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Gxu/Types.h"

class EMBEDDEDPARSEINFO {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ GXUFONTHYPERLINKINFO m_hyperlink;
    /* 002C */ EMBEDDEDTEXTUREINFO m_texture;
};

#endif
