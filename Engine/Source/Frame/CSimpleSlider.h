#ifndef ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H
#define ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CSimpleSlider : public CSimpleFrame {
    public:
    /* 029C */ uint32_t m_sliderFlags;
    /* 02A0 */ C2Vector m_minMax;
    /* 02A8 */ uint32_t m_value;
    /* 02AC */ uint32_t m_valueStep;
    /* 02B0 */ uint32_t m_simpleSliderUkn6;
    /* 02B4 */ uint32_t m_simpleSliderUkn7;
    /* 02B8 */ uint32_t m_simpleSliderUkn8;
    /* 02BC */ uint32_t m_simpleSliderUkn9;
    /* 02C0 */ uint32_t m_simpleSliderUkn10;
    /* 02C4 */ uint32_t m_simpleSliderUkn11;
};

#endif
