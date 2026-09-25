#ifndef ENGINE_SOURCE_TEMPEST_CIMVECTOR_H
#define ENGINE_SOURCE_TEMPEST_CIMVECTOR_H

#include <cstddef>
#include <cstdint>

class C3Vector;

class CImVector {
    public:
    /* 0000 */ uint8_t b;
    /* 0001 */ uint8_t g;
    /* 0002 */ uint8_t r;
    /* 0003 */ uint8_t a;

    CImVector();
    void InterpColor(int32_t t, const CImVector* src);
    void Set(float a, float r, float g, float b);
    CImVector& operator=(const C3Vector& color);
};

// INLINED
inline CImVector::CImVector() {
    this->b = 0;
    this->g = 0;
    this->r = 0;
    this->a = 0;
}

#endif
