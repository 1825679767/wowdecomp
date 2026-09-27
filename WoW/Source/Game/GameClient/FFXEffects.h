#ifndef WOW_SOURCE_GAME_GAMECLIENT_FFXEFFECTS_H
#define WOW_SOURCE_GAME_GAMECLIENT_FFXEFFECTS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class EffectDeath : public FFX::Effect {
    public:
};

class EffectNetherWorld {
    public:
    /* 0000 */ uint8_t unk_0000 [12];
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
};

class EffectSwirlingFog {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ uint8_t unk_0008 [4];
    /* 000C */ int32_t unk_000C;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ uint8_t unk_0014 [16];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ uint8_t unk_002C[4];
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
};

#endif
