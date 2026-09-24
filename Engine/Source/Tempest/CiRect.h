#ifndef ENGINE_SOURCE_TEMPEST_CIRECT_H
#define ENGINE_SOURCE_TEMPEST_CIRECT_H

#include <cstddef>
#include <cstdint>

class CiRect {
    public:
    /* 0000 */ int32_t minY;
    /* 0004 */ int32_t minX;
    /* 0008 */ int32_t maxY;
    /* 000C */ int32_t maxX;

    CiRect operator&=(const CiRect& r);
};

CiRect operator&(const CiRect& l, const CiRect& r);

#endif
