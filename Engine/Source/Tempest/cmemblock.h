#ifndef ENGINE_SOURCE_TEMPEST_CMEMBLOCK_H
#define ENGINE_SOURCE_TEMPEST_CMEMBLOCK_H

#include <cstddef>
#include <cstdint>

class CMemBlock {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
};

#endif
