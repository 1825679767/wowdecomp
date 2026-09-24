#ifndef ENGINE_SOURCE_TEMPEST_NTEMPEST_H
#define ENGINE_SOURCE_TEMPEST_NTEMPEST_H

#include <cstddef>
#include <cstdint>

class C2Vector;
class C3Vector;
class CAaBox;

namespace NTempest {
    float Distance(const CAaBox& box, const C3Vector& point);
    float DistanceSquared(const CAaBox& box, const C2Vector& point);
    float DistanceSquared(const CAaBox& box, const C3Vector& point);
}

#endif
