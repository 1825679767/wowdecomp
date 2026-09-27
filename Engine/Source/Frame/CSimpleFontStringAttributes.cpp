#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFontStringAttributes.h"

// VERIFY: ec752594 auto
// OFFSET: 0x00496390
void CSimpleFontStringAttributes::SetJustifyH(uint8_t justify) {
    uint32_t styleFlags = (this->m_styleFlags & ~0x7u) | (justify & 0x7);

    if (styleFlags != this->m_styleFlags || !(this->m_flags & 0x200)) {
        this->m_flags |= 0x202;
        this->m_styleFlags = styleFlags;
    }
}

// VERIFY: 15b1ef9e auto
// OFFSET: 0x004963C0
void CSimpleFontStringAttributes::SetJustifyV(uint8_t justify) {
    uint32_t styleFlags = (this->m_styleFlags & ~0x38u) | (justify & 0x38);

    if (styleFlags != this->m_styleFlags || !(this->m_flags & 0x200)) {
        this->m_flags |= 0x202;
        this->m_styleFlags = styleFlags;
    }
}
