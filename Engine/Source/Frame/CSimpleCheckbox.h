#ifndef ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H
#define ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CSimpleButton.h"

class CSimpleCheckbox : public CSimpleButton {
    public:
    /* 02F8 */ uint32_t m_isChecked;
    /* 02FC */ uint32_t m_checkedTexture;
    /* 0300 */ uint32_t m_disabledTexture;
};

#endif
