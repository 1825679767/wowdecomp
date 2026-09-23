#ifndef WOW_SOURCE_GAME_NAMEGEN_H
#define WOW_SOURCE_GAME_NAMEGEN_H

#include <cstddef>
#include <cstdint>

class CNameGen {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint8_t unk_000C[8];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[4];
    /* 001C */ uint32_t unk_001C;
};

#endif
