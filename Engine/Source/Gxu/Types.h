#ifndef ENGINE_SOURCE_GXU_TYPES_H
#define ENGINE_SOURCE_GXU_TYPES_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct GXUFONTHYPERLINKINFO {
    /* 0000 */ float m_lineHeight;
    /* 0004 */ float m_indentation;
    /* 0008 */ float m_baseLineHeight;
    /* 000C */ float unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
};

#endif
