#include <cmath>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/NTempest.h"

// VERIFY: c57f24fc manual
// OFFSET: 0x0078F3E0
float NTempest::Distance(const CAaBox& box, const C3Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float z = box.b.z <= point.z ? (box.t.z >= point.z ? point.z : box.t.z) : box.b.z;
    float dy = y - point.y;
    return sqrt(dy * dy + (x - point.x) * (x - point.x) + (z - point.z) * (z - point.z));
}

// VERIFY: 0b6b7f8b manual
// OFFSET: 0x007B4830
float NTempest::DistanceSquared(const CAaBox& box, const C2Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float dy = y - point.y;
    return (x - point.x) * (x - point.x) + dy * dy;
}

// VERIFY: d96a0cf4 manual
// OFFSET: 0x007B48C0
float NTempest::DistanceSquared(const CAaBox& box, const C3Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float z = box.b.z <= point.z ? (box.t.z >= point.z ? point.z : box.t.z) : box.b.z;
    float dy = y - point.y;
    float dz = z - point.z;
    return dy * dy + (x - point.x) * (x - point.x) + dz * dz;
}
