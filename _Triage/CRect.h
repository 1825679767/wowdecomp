#ifndef _TRIAGE_CRECT_H
#define _TRIAGE_CRECT_H

#include <cstddef>
#include <cstdint>

class CRect {
    public:
    /* 0000 */ float m_minY;
    /* 0004 */ float m_minX;
    /* 0008 */ float m_maxY;
    /* 000C */ float m_maxX;
};

#endif
