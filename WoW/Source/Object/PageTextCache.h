#ifndef WOW_SOURCE_OBJECT_PAGETEXTCACHE_H
#define WOW_SOURCE_OBJECT_PAGETEXTCACHE_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class PageTextCache {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
};

class PageTextCache_C {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;

    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif
