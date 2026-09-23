#ifndef WOW_SOURCE_WORLDCLIENT_BSPQUERY_SEGMENTLINK_H
#define WOW_SOURCE_WORLDCLIENT_BSPQUERY_SEGMENTLINK_H

#include <cstddef>
#include <cstdint>

class BspQuery_SegmentLink {
    public:
    /* 0000 */ uint8_t unk_0000[48];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint8_t unk_0034[20];
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
};

#endif
