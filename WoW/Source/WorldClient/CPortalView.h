#ifndef WOW_SOURCE_WORLDCLIENT_CPORTALVIEW_H
#define WOW_SOURCE_WORLDCLIENT_CPORTALVIEW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CRect.h"

class C3Vector;

class CPortalView {
    public:
    /* 0000 */ CRect m_rect;
    /* 0010 */ float m_maxViewDepth;
    /* 0014 */ C3Vector* m_verts;
    /* 0018 */ uint32_t m_vertCount;
};

#endif
