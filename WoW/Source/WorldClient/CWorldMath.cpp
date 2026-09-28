#include "CWorldMath.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include <cmath>

// VERIFY: a0e6ed09 manual
// OFFSET: 0x007A61D0
int32_t CWorldMath::ComputeAaBoxOutcode(CAaBox* box, C3Vector* point) {
    float d0 = point->x - box->b.x + 0.019444443f;
    float d1 = point->y - box->b.y + 0.019444443f;
    float d2 = point->z - box->b.z + 0.019444443f;
    float d3 = box->t.x - point->x + 0.019444443f;
    float d4 = box->t.y - point->y + 0.019444443f;
    float d5 = box->t.z - point->z + 0.019444443f;

    uint32_t outcode = *reinterpret_cast<uint32_t*>(&d0) >> 31;
    outcode |= (*reinterpret_cast<uint32_t*>(&d1) >> 30) & 0x02;
    outcode |= (*reinterpret_cast<uint32_t*>(&d2) >> 29) & 0x04;
    outcode |= (*reinterpret_cast<uint32_t*>(&d3) >> 28) & 0x08;
    outcode |= (*reinterpret_cast<uint32_t*>(&d4) >> 27) & 0x10;
    outcode |= (*reinterpret_cast<uint32_t*>(&d5) >> 26) & 0x20;

    return outcode;
}

// VERIFY: fc48135f auto
// OFFSET: 0x007F9320
void CWorldMath::TransformAABoxInner(const float* row1, const float* row0, CAaBox& dst, CAaBox& src, const float* row2) {
    const float* rows[3] = { row0, row1, row2 };

    for (int32_t axis = 0; axis < 3; axis++) {
        for (int32_t component = 0; component < 3; component++) {
            float e = rows[component][axis];

            float a = e * (&src.b.x)[component];
            float b = e * (&src.t.x)[component];

            (&dst.b.x)[axis] += a < b ? a : b;
            (&dst.t.x)[axis] += a < b ? b : a;
        }
    }
}

// VERIFY: 51bf69b0 manual
// OFFSET: 0x007F93D0
void CWorldMath::TransformAABox(C33Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = { 0.0f, 0.0f, 0.0f };
    dst.t = { 0.0f, 0.0f, 0.0f };

    CWorldMath::TransformAABoxInner(&m.b0, &m.a0, dst, src, &m.c0);
}

// VERIFY: 07b42bca manual
// OFFSET: 0x007F9430
void CWorldMath::TransformAABox(C44Matrix& m, CAaBox& src, CAaBox& dst) {
    dst.b = C3Vector(m.d0, m.d1, m.d2);
    dst.t = C3Vector(m.d0, m.d1, m.d2);

    CWorldMath::TransformAABoxInner(&m.b0, &m.a0, dst, src, &m.c0);
}

// VERIFY: 011d2b0a manual
// OFFSET: 0x007F9480
bool CWorldMath::VectorIntersectAABox2(CAaBox& box, C3Vector& start, C3Vector& end) {
    const float* minB = &box.b.x;
    const float* maxB = &box.t.x;
    const float* origin = &start.x;
    const float* target = &end.x;

    float hit[3];
    float dir[3];
    float maxT[3];

    dir[0] = end.x - start.x;
    dir[1] = end.y - start.y;
    dir[2] = end.z - start.z;
    maxT[0] = -1.0f;
    maxT[1] = -1.0f;
    maxT[2] = -1.0f;

    int32_t inside = 1;

    for (uint32_t i = 0; i < 3; i++) {
        if (origin[i] < minB[i]) {
            if (target[i] < minB[i]) {
                return false;
            }

            hit[i] = minB[i];
            inside = 0;

            if (dir[i] != 0.0f) {
                maxT[i] = (minB[i] - origin[i]) / dir[i];
            }
        } else if (origin[i] > maxB[i]) {
            if (target[i] > maxB[i]) {
                return false;
            }

            hit[i] = maxB[i];
            inside = 0;

            if (dir[i] != 0.0f) {
                maxT[i] = (maxB[i] - origin[i]) / dir[i];
            }
        }
    }

    if (inside) {
        return true;
    }

    uint32_t plane = maxT[0] < maxT[1];

    if (maxT[plane] < maxT[2]) {
        plane = 2;
    }

    if (*reinterpret_cast<uint32_t*>(&maxT[plane]) & 0x80000000) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (i != plane) {
            hit[i] = origin[i] + maxT[plane] * dir[i];

            if (minB[i] - 0.00001f > hit[i] || maxB[i] + 0.00001f < hit[i]) {
                return false;
            }
        }
    }

    return true;
}
