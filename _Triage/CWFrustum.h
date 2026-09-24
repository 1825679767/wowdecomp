#ifndef _TRIAGE_CWFRUSTUM_H
#define _TRIAGE_CWFRUSTUM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Storm/H/stpl.h"

class CWFrustum;

class CWFrustum {
    public:
    /* 0000 */ C4Plane m_planes [6];
    /* 0060 */ C3Vector m_corners [8];
    /* 00C0 */ C3Vector m_lookPos;
    /* 00CC */ C3Vector m_lookAt;
    /* 00D8 */ C3Vector m_lookUp;
    /* 00E4 */ float m_fovy;
    /* 00E8 */ float m_aspect;
    /* 00EC */ float m_minz;
    /* 00F0 */ float m_maxz;
    /* 00F4 */ TSLink<CWFrustum> m_sceneLink;
};

#endif
