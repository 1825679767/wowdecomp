#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CImVector.h"

// VERIFY: 29546a0f manual
// OFFSET: 0x0048BD20
void CImVector::Set(float a, float r, float g, float b) {
    this->a = static_cast<uint8_t>(CMath::ftol_round_0_255_(a));
    this->r = static_cast<uint8_t>(CMath::ftol_round_0_255_(r));
    this->g = static_cast<uint8_t>(CMath::ftol_round_0_255_(g));
    this->b = static_cast<uint8_t>(CMath::ftol_round_0_255_(b));
}

// VERIFY: 5249a0a5 manual
// OFFSET: 0x009851A0
CImVector& CImVector::operator=(const C3Vector& color) {
    this->a = 0xFF;
    this->r = static_cast<uint8_t>(CMath::fint_(color.x * 255.0f));
    this->g = static_cast<uint8_t>(CMath::fint_(color.y * 255.0f));
    this->b = static_cast<uint8_t>(CMath::fint_(color.z * 255.0f));
    return *this;
}
