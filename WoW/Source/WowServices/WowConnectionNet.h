#ifndef WOW_SOURCE_WOWSERVICES_WOWCONNECTIONNET_H
#define WOW_SOURCE_WOWSERVICES_WOWCONNECTIONNET_H

#include <cstddef>
#include <cstdint>

class WowConnectionNet {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010[16];
    /* 0020 */ uint8_t unk_0020;
    /* 0021 */ uint8_t unk_0021[3];
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint8_t unk_0028[2192];
    /* 08B8 */ uint32_t unk_08B8;
    /* 08BC */ uint32_t unk_08BC;
    /* 08C0 */ uint32_t unk_08C0;
    /* 08C4 */ uint8_t unk_08C4[4];
    /* 08C8 */ uint32_t unk_08C8;
    /* 08CC */ uint32_t unk_08CC;
    /* 08D0 */ uint32_t unk_08D0;
    /* 08D4 */ uint32_t unk_08D4;
    /* 08D8 */ uint32_t unk_08D8;
    /* 08DC */ uint32_t unk_08DC;
    /* 08E0 */ uint8_t unk_08E0[4];
    /* 08E4 */ uint32_t unk_08E4;
    /* 08E8 */ uint32_t unk_08E8;
};

#endif
