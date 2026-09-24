#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// VERIFY: 3e0f9ba5 manual
// OFFSET: 0x0048BD20
void CImVector::Set(float a, float r, float g, float b) {
    this->a = static_cast<uint8_t>(static_cast<int32_t>(a * 255.0f + 0.5f));
    this->r = static_cast<uint8_t>(static_cast<int32_t>(r * 255.0f + 0.5f));
    this->g = static_cast<uint8_t>(static_cast<int32_t>(g * 255.0f + 0.5f));
    this->b = static_cast<uint8_t>(static_cast<int32_t>(255.0f * b + 0.5f));
}

// VERIFY: 81b97ccb auto
// OFFSET: 0x009851A0
CImVector& CImVector::operator=(const C3Vector& color) {
    this->a = 0xFF;
    this->r = static_cast<uint8_t>(static_cast<int32_t>(color.x * 255.0f));
    this->g = static_cast<uint8_t>(static_cast<int32_t>(color.y * 255.0f));
    this->b = static_cast<uint8_t>(static_cast<int32_t>(color.z * 255.0f));
    return *this;
}
