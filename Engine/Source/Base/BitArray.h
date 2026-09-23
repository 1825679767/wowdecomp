#ifndef ENGINE_SOURCE_BASE_BITARRAY_H
#define ENGINE_SOURCE_BASE_BITARRAY_H

#include <cstddef>
#include <cstdint>

class CBitArray {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009[23];
    /* 0020 */ uint8_t unk_0020;
};

#endif
