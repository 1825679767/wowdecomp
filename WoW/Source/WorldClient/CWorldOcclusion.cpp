#include "CWorldOcclusion.h"

// STATIC: 0x00AEEF60
TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> s_occluderFreeList;

// STATIC: 0x00AEEF6C
TSExplicitList<CWorldAntiOccluder, offsetof(CWorldAntiOccluder, m_link)> s_antiOccluderFreeList;

// VERIFY: f52cc1f6 manual
// OFFSET: 0x005EEB70
void CWorldOccluder::Initialize() {
}
