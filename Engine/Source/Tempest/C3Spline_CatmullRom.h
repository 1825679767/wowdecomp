#ifndef ENGINE_SOURCE_TEMPEST_C3SPLINE_CATMULLROM_H
#define ENGINE_SOURCE_TEMPEST_C3SPLINE_CATMULLROM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/c3spline.h"

class C3Spline_CatmullRom : public C3Spline {
    public:
    /* 01C0 */ uint32_t m_splineMode;
};

#endif
