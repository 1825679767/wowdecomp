#ifndef _TRIAGE_CWFRUSTUM_H
#define _TRIAGE_CWFRUSTUM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Storm/H/stpl.h"

class CWFrustum;

class CWFrustum : public CFrustum {
    public:
    /* 00E4 */ float m_fovy;
    /* 00E8 */ float m_aspect;
    /* 00EC */ float m_minz;
    /* 00F0 */ float m_maxz;
    /* 00F4 */ TSLink<CWFrustum> m_sceneLink;
};

#endif
