#ifndef WOW_SOURCE_BATTLENET_TUMOR_TUMOR_H
#define WOW_SOURCE_BATTLENET_TUMOR_TUMOR_H

#include <cstddef>
#include <cstdint>

namespace Battlenet {
struct Tumor {
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001[4];
    /* 0005 */ uint8_t unk_0005 [16];
    /* 0015 */ uint8_t unk_0015[4];
    /* 0019 */ uint8_t unk_0019 [20];
    /* 002D */ uint8_t unk_002D[4];
    /* 0031 */ uint8_t unk_0031 [76];
    /* 007D */ uint8_t unk_007D[4];
    /* 0081 */ uint8_t unk_0081 [35];
    /* 00A4 */ int32_t unk_00A4;
};
}

#endif
