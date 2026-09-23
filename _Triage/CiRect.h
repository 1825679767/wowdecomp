#ifndef _TRIAGE_CIRECT_H
#define _TRIAGE_CIRECT_H

#include <cstddef>
#include <cstdint>

class CiRect {
    public:
    /* 0000 */ int32_t m_minY;
    /* 0004 */ int32_t m_minX;
    /* 0008 */ int32_t m_maxY;
    /* 000C */ int32_t m_maxX;
};

#endif
