#ifndef ENGINE_SOURCE_FFX_FFX_H
#define ENGINE_SOURCE_FFX_FFX_H

#include <cstddef>
#include <cstdint>

struct Effect {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ int32_t m_svFfxGlow;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
};

class FFX {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint8_t unk_002C[132];
    /* 00B0 */ uint8_t unk_00B0;
};

#endif
