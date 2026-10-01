#ifndef ENGINE_SOURCE_TEMPEST_CFRUSTUM_H
#define ENGINE_SOURCE_TEMPEST_CFRUSTUM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"

class C44Matrix;
class CAaBox;
class CAaSphere;

class CFrustum {
    public:
    /* 0000 */ C4Plane planes[6];
    /* 0060 */ C3Vector corners[8];
    /* 00C0 */ C3Vector lookPos;
    /* 00CC */ C3Vector lookAt;
    /* 00D8 */ C3Vector lookUp;
    /* 00E4 */ float fovy;
    /* 00E8 */ float aspect;
    /* 00EC */ float minz;
    /* 00F0 */ float maxz;

    CFrustum();
    CFrustum(const C3Vector* corners);
    void CalcPlanesFromCorners();
    void CalcPlanesFromCorners(const C3Vector* corners);
    uint32_t Cull(const CAaBox& box);
    uint32_t Cull(const CAaSphere& sphere);
    void Cull(const C3Vector& point, uint8_t& outside) const;
    uint32_t Cull(const C3Vector& center, float radius);
    uint32_t CullInv(const CAaBox& box);
    void Extents(CAaBox& box);
    void InvertPlanes();
    void SwapCorners();
    void Transform(const C44Matrix& m);
    void Translate(const C3Vector& v);
    CFrustum& operator=(const CFrustum& source);
};

#endif
