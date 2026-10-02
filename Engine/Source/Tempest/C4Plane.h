#ifndef ENGINE_SOURCE_TEMPEST_C4PLANE_H
#define ENGINE_SOURCE_TEMPEST_C4PLANE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C4Plane {
    public:
    /* 0000 */ C3Vector n;
    /* 000C */ float d;

    C4Plane();
    void From3Pos(const C3Vector& p0, const C3Vector& p1, const C3Vector& p2);
};

// INLINED
inline C4Plane::C4Plane() {
    this->n.x = 0.0f;
    this->n.y = 0.0f;
    this->n.z = 1.0f;
    this->d = 0.0f;
}

#endif
