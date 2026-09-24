#ifndef _TRIAGE_CFACET_H
#define _TRIAGE_CFACET_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/c4plane.h"
#include "Engine/Source/Storm/H/stpl.h"

class CFacet;

class CFacet {
    public:
    /* 0000 */ C4Plane m_plane;
    /* 0010 */ C3Vector m_v [3];
};

struct FacetData {
    /* 0000 */ TSGrowableArray<CFacet> m_facets;
    /* 0010 */ TSGrowableArray<uint64_t> m_facetIds;
};

#endif
