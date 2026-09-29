#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H

#include <cstddef>
#include <cstdint>

class PassFogSeed {
    public:
    static void __fastcall DeviceRestoredCallback();

    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[28];
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint8_t unk_002C[4];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
};

#endif
