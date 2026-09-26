#ifndef ENGINE_SOURCE_TEMPEST_CMATH_H
#define ENGINE_SOURCE_TEMPEST_CMATH_H

#include <cmath>
#include <cstddef>
#include <cstdint>

class CMath {
    public:
    static double exp2_(double x);
    static int32_t fint_(float f);
    static int32_t fint_n(float f);
    static int32_t ftol_round_0_255_(float f);
    static double log2_(double x);
    static float normalizeAngleNegPiToPi_(float angle);
    static float normalizeangle0to2pi_(float angle);
    static bool solvequad_(float a, float b, float c, float& r0, float& r1);
    static uint32_t fuint_pi(float f);
    static uint32_t sqrt_(uint32_t n);
};

bool fequal(float a, float b);
int32_t fequalz(float a, float b, float z);
uint32_t CeilToInt64(float value);
int32_t FloatToInt64(float value);
bool fnotequal(float a, float b);
int32_t IsNaN(double value);

// INLINED
inline int32_t CMath::fint_(float f) {
    return static_cast<int32_t>(lrintf(f));
}

// INLINED
inline int32_t CMath::ftol_round_0_255_(float f) {
    return static_cast<int32_t>(f * 255.0f + 0.5f);
}

#endif
