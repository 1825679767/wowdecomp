#ifndef ENGINE_SOURCE_TEMPEST_CIMVECTOR_H
#define ENGINE_SOURCE_TEMPEST_CIMVECTOR_H

#include <cstddef>
#include <cstdint>

class CImVector {
    public:
    /* 0000 */ uint8_t m_b;
    /* 0001 */ uint8_t m_g;
    /* 0002 */ uint8_t m_r;
    /* 0003 */ uint8_t m_a;
};

#endif
