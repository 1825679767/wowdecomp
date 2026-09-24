#ifndef ENGINE_SOURCE_TEMPEST_CRANDOM_H
#define ENGINE_SOURCE_TEMPEST_CRANDOM_H

#include <cstddef>
#include <cstdint>

class CRndSeed {
    public:
    /* 0000 */ uint32_t rndacc;
    /* 0004 */ uint32_t rndvls;
};

class CRandom {
    public:
    static uint32_t uint32_(CRndSeed& seed);
};

#endif
