#ifndef WOW_SOURCE_UIUTIL_CSIMPLECAMERA_H
#define WOW_SOURCE_UIUTIL_CSIMPLECAMERA_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"

class CSimpleCamera {
    public:
    /* 0004 */ float unk_0004;
    /* 0008 */ C3Vector m_position;
    /* 0014 */ C33Matrix m_facing;
    /* 0038 */ float m_nearZ;
    /* 003C */ float m_farZ;
    /* 0040 */ float m_fov;
    /* 0044 */ float m_aspect;

    virtual float FOV() const;
    virtual C3Vector Forward() const;
    virtual C3Vector Right() const;
    virtual C3Vector Up() const;
};

#endif
