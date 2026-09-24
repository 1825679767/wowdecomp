#include <cstdint>
#include "Engine/Source/Gx/CGxFormat.h"

// VERIFY: f33003a7 auto
// OFFSET: 0x00681950
CGxFormat::CGxFormat() {
    this->m_size.x = 0;
    this->m_size.y = 0;
    this->m_pos.x = 0;
    this->m_pos.y = 0;
    this->m_multisampleQuality = 0.0f;
    this->m_maximize = 0;
    this->m_stereoEnabled = false;
    this->m_multisampleCount = 1;
    this->m_aspect = true;

    for (uint32_t i = 0; i < 6; i++) {
        this->m_fixedFunction[i] = -1;
    }
}
