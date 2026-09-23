#ifndef ENGINE_SOURCE_BLPFILE_TYPES_H
#define ENGINE_SOURCE_BLPFILE_TYPES_H

#include <cstddef>
#include <cstdint>

struct C4Pixel;

struct BlpPalPixel {
    /* 0000 */ uint8_t m_b;
    /* 0001 */ uint8_t m_g;
    /* 0002 */ uint8_t m_r;
    /* 0003 */ uint8_t m_pad;
};

struct MipBits {
    /* 0000 */ C4Pixel* m_mip [1];
};

enum MipMapAlgorithm {
    MMA_BOX = 0,
    MMA_CUBIC = 1,
    MMA_FULLDFT = 2,
    MMA_KAISER = 3,
    MMA_LINEARLIGHTKAISER = 4,
};

#endif
