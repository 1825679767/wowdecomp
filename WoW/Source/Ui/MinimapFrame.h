#ifndef WOW_SOURCE_UI_MINIMAPFRAME_H
#define WOW_SOURCE_UI_MINIMAPFRAME_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleFrame.h"

class CSimpleTexture;

class CGMinimapFrame : public CSimpleFrame {
    public:
    /* 029C */ uint32_t m_minimapUkn1;
    /* 02A0 */ CSimpleTexture* m_someTexture;
    /* 02A4 */ uint32_t m_minimapUkn3;
    /* 02A8 */ uint32_t m_minimapUkn4;
    /* 02AC */ uint32_t m_minimapUkn5;
};

#endif
