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

    void Set(float a, float r, float g, float b);
    CImVector& operator=(const C3Vector& color);
};

#endif
