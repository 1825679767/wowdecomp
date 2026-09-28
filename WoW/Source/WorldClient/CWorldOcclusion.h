#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDOCCLUSION_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDOCCLUSION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CWorldOccluder {
    public:
    static void Initialize();

    /* 0000 */ uint8_t unk_0000[24];
    /* 0018 */ TSLink<CWorldOccluder> m_link;
};

class CWorldAntiOccluder {
    public:
    /* 0000 */ uint8_t unk_0000[16];
    /* 0010 */ TSLink<CWorldAntiOccluder> m_link;
};

extern TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> s_occluderFreeList;
extern TSExplicitList<CWorldAntiOccluder, offsetof(CWorldAntiOccluder, m_link)> s_antiOccluderFreeList;

class CClipVolume {
    public:
    /* 0000 */ uint32_t m_planeStart;
    /* 0004 */ uint32_t m_planeCount;
};

#endif
