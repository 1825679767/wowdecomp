#ifndef ENGINE_SOURCE_TEMPEST_C4PLANE_H
#define ENGINE_SOURCE_TEMPEST_C4PLANE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C4Plane {
    public:
    /* 0000 */ C3Vector m_n;
    /* 000C */ float m_d;
};

#endif
