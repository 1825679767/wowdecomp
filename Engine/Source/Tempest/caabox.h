#ifndef ENGINE_SOURCE_TEMPEST_CAABOX_H
#define ENGINE_SOURCE_TEMPEST_CAABOX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CAaBox {
    public:
    /* 0000 */ C3Vector m_b;
    /* 000C */ C3Vector m_t;
};

struct M2Bounds {
    /* 0000 */ CAaBox m_extent;
    /* 0018 */ float m_radius;
};

#endif
