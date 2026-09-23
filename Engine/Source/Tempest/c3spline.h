#ifndef ENGINE_SOURCE_TEMPEST_C3SPLINE_H
#define ENGINE_SOURCE_TEMPEST_C3SPLINE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Storm/H/stpl.h"

class C3Vector;

class C3Spline {
    public:
    /* 0004 */ float m_length;
    /* 0008 */ C3Vector m_points [25];
    /* 0134 */ TSGrowableArray<C3Vector> m_extraPoints;
    /* 0144 */ uint32_t m_pointCount;
    /* 0148 */ float m_segLength [25];
    /* 01AC */ TSGrowableArray<float> m_extraSegLength;
    /* 01BC */ uint32_t m_segCount;

    virtual ~C3Spline();
};

#endif
