#ifndef _TRIAGE_CWFRUSTUM_H
#define _TRIAGE_CWFRUSTUM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Storm/H/stpl.h"

class CWFrustum;

class CWFrustum : public CFrustum {
    public:
    /* 00F4 */ TSLink<CWFrustum> m_link;
};

#endif
