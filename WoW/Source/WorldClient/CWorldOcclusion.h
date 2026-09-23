#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDOCCLUSION_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDOCCLUSION_H

#include <cstddef>
#include <cstdint>

class CClipVolume {
    public:
    /* 0000 */ uint32_t m_planeStart;
    /* 0004 */ uint32_t m_planeCount;
};

#endif
