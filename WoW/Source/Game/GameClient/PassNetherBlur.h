#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H

#include <cstddef>
#include <cstdint>

class PassNetherBlur {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[8];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010[141];
    /* 009D */ uint8_t unk_009D;
    /* 009E */ uint8_t unk_009E;
    /* 009F */ uint8_t unk_009F;
    /* 00A0 */ uint32_t unk_00A0;
};

#endif
