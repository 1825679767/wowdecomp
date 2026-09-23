#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VEHICLE_C_H

#include <cstddef>
#include <cstdint>

class CVehicle {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[4];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010[72];
    /* 0058 */ uint32_t unk_0058;
};

class CVehicle_C {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[272];
    /* 0160 */ uint32_t unk_0160;
    /* 0164 */ uint32_t unk_0164;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint8_t unk_016C[4];
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint8_t unk_0174[4];
    /* 0178 */ uint32_t unk_0178;
};

class UnitF54 {
    public:
    /* 0000 */ uint8_t unk_0000[12];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint8_t unk_0010[348];
    /* 016C */ uint8_t unk_016C;
    /* 016D */ uint8_t unk_016D;
    /* 016E */ uint8_t unk_016E[2];
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint8_t unk_0174[4];
    /* 0178 */ uint32_t unk_0178;
};

#endif
