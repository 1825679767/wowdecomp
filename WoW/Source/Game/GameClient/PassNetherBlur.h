#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class PassNetherBlur : public FFX::Pass {
    public:
    /* 0030 */ uint8_t unk_0030[616];

    ~PassNetherBlur() override;
    bool Valid() override;
    void Execute() override;

    PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk);
};

class PassNetherCombine : public FFX::Pass {
    public:
    /* 0030 */ uint8_t unk_0030[16];

    ~PassNetherCombine() override;
    bool Valid() override;
    void Execute() override;

    PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk);
};

#endif
