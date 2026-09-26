#include "Engine/Source/Model2/M2Data.h"

// VERIFY: f762ece4 auto
// OFFSET: 0x0081CE60
ubyte4::operator uint32_t() const {
    return *reinterpret_cast<const uint32_t*>(this->b);
}

// VERIFY: 501d6d87 auto
// OFFSET: 0x008248B0
uint8_t ubyte4::operator[](uint32_t index) const {
    return this->b[index];
}
