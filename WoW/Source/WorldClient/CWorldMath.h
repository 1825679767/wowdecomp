#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H

#include <cstdint>

class C33Matrix;
class C3Vector;
class C44Matrix;
class CAaBox;

class CWorldMath {
    public:
    static void TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst);
    static void TransformAABoxInner(const float* row1, const float* row0, CAaBox& dst, CAaBox& src, const float* row2);
    static void TransformAABox(C33Matrix& m, CAaBox& src, CAaBox& dst);
    static bool VectorIntersectAABox2(CAaBox& box, C3Vector& start, C3Vector& end);
    static int32_t ComputeAaBoxOutcode(CAaBox* box, C3Vector* point);
};

#endif
