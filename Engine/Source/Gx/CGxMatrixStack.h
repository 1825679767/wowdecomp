#ifndef ENGINE_SOURCE_GX_CGXMATRIXSTACK_H
#define ENGINE_SOURCE_GX_CGXMATRIXSTACK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/c44matrix.h"

class CGxMatrixStack {
    public:
    /* 0000 */ uint32_t m_level;
    /* 0004 */ int8_t m_dirty;
    /* 0005 */ uint8_t unk_0005[3];
    /* 0008 */ C44Matrix m_mtx [4];
    /* 0108 */ uint32_t m_flags [4];
};

#endif
