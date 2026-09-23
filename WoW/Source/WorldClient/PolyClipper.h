#ifndef WOW_SOURCE_WORLDCLIENT_POLYCLIPPER_H
#define WOW_SOURCE_WORLDCLIENT_POLYCLIPPER_H

#include <cstddef>
#include <cstdint>

class PCResultsIterator {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014;
};

class PolyClipper {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[8];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint8_t unk_0020[48];
    /* 0050 */ uint8_t unk_0050;
    /* 0051 */ uint8_t unk_0051[1519];
    /* 0640 */ uint32_t unk_0640;
    /* 0644 */ uint32_t unk_0644;
    /* 0648 */ uint32_t unk_0648;
    /* 064C */ uint32_t unk_064C;
};

#endif
