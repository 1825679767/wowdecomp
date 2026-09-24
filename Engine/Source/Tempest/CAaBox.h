#ifndef ENGINE_SOURCE_TEMPEST_CAABOX_H
#define ENGINE_SOURCE_TEMPEST_CAABOX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C44Matrix;

class CAaBox {
    public:
    static CAaBox Bounding(const C3Vector* points, uint32_t count);
    static CAaBox Transform(const CAaBox& box, const C44Matrix& m);

    /* 0000 */ C3Vector b;
    /* 000C */ C3Vector t;

    CAaBox() = default;
    CAaBox(const C3Vector& b, const C3Vector& t);
    bool ContainsPoint(const C3Vector& point) const;
    bool Intersects(const CAaBox& box) const;
    bool IntersectsXY(const CAaBox& box) const;
    bool IsEmpty() const;
    bool IsValid() const;
    CAaBox operator|=(const CAaBox& box);
};

CAaBox operator|(const CAaBox& l, const CAaBox& r);

#endif
