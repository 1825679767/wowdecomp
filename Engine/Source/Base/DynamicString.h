#ifndef ENGINE_SOURCE_BASE_DYNAMICSTRING_H
#define ENGINE_SOURCE_BASE_DYNAMICSTRING_H

#include <cstddef>
#include <cstdint>

class CDynamicString {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
};

class DynamicString {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint16_t unk_0002;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
};

#endif
