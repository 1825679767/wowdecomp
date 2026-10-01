#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSFOGSEED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class PassFogSeed : public FFX::Pass {
    public:
    static void __fastcall DeviceRestoredCallback();

    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;

    ~PassFogSeed() override;
    bool Valid() override;
    void Execute() override;

    PassFogSeed(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk);
};

#endif
