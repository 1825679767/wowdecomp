#ifndef _TRIAGE_CFACET_H
#define _TRIAGE_CFACET_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Storm/H/stpl.h"

struct FacetData {
    /* 0000 */ TSGrowableArray<CFacet> m_facets;
    /* 0010 */ TSGrowableArray<uint64_t> m_facetIds;
};

#endif
