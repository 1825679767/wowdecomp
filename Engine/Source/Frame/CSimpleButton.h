#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_H
#define ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"

struct CSimpleFont;
struct CSimpleFontString;
struct CSimpleTexture;

class CSimpleButton : public CSimpleFrame {
    public:
    /* 029C */ int32_t m_buttonUnk1;
    /* 02A0 */ int64_t m_buttonMask;
    /* 02A8 */ int32_t m_state;
    /* 02AC */ CSimpleFontString* m_fontString;
    /* 02B0 */ CSimpleFont* m_normalFont;
    /* 02B4 */ CSimpleFont* m_highlightFont;
    /* 02B8 */ CSimpleFont* m_disabledFont;
    /* 02BC */ C2Vector m_fontOffset;
    /* 02C4 */ CSimpleTexture* m_stateTextures [3];
    /* 02D0 */ CSimpleTexture* m_activeTexture;
    /* 02D4 */ int32_t m_highlightTexture;
    /* 02D8 */ int32_t m_onPreClick;
    /* 02DC */ int32_t m_buttonUnk17;
    /* 02E0 */ int32_t m_onClick;
    /* 02E4 */ int32_t m_buttonUnk19;
    /* 02E8 */ int32_t m_onPostClick;
    /* 02EC */ int32_t m_buttonUnk21;
    /* 02F0 */ int32_t m_onDoubleClick;
    /* 02F4 */ int32_t m_buttonUnk23;
};

#endif
