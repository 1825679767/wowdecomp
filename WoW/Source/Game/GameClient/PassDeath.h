#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSDEATH_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSDEATH_H

#include <cstddef>
#include <cstdint>

class PassDeath {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ uint8_t m_gapC [24];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ uint8_t unk_0028 [4];
    /* 002C */ char unk_002C;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ int32_t unk_0030;
};

#endif
