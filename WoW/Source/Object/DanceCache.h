#ifndef WOW_SOURCE_OBJECT_DANCECACHE_H
#define WOW_SOURCE_OBJECT_DANCECACHE_H

#include <cstddef>
#include <cstdint>

class DanceCache {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[12];
    /* 0010 */ uint64_t unk_0010;
    /* 0018 */ uint8_t unk_0018[120];
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint64_t unk_0098;
};

#endif
