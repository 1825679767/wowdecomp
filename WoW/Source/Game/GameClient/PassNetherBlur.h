#ifndef WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H
#define WOW_SOURCE_GAME_GAMECLIENT_PASSNETHERBLUR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Tempest/CRandom.h"

class CGxShader;

class PassNetherBlur : public FFX::Pass {
    public:
    /* 0030 */ uint32_t m_useRect;
    /* 0034 */ float m_noise[3][36];
    /* 01E4 */ float unk_01E4[36];
    /* 0274 */ float* m_noiseTables[3];
    /* 0280 */ float unk_0280;
    /* 0284 */ CRndSeed m_seed;
    /* 028C */ CGxShader* m_vertexShader;
    /* 0290 */ CGxShader* m_pixelShader;

    ~PassNetherBlur() override;
    bool Valid() override;
    void Execute() override;

    PassNetherBlur(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk);
};

class PassNetherCombine : public FFX::Pass {
    public:
    /* 0030 */ uint32_t m_useRect;
    /* 0034 */ CGxShader* m_pixelShader;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ float unk_003C;

    ~PassNetherCombine() override;
    bool Valid() override;
    void Execute() override;

    PassNetherCombine(FFX::Tex** inputs, uint32_t count, FFX::Tex* output, EGxBlend blend, bool unk);
};

#endif
