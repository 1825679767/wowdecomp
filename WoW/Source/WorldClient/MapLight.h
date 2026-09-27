#ifndef WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H
#define WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Model2/M2Light.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CMapLight : public CMapBaseObj {
    public:
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ float unk_0030;
    /* 0034 */ float unk_0034;
    /* 0038 */ float unk_0038;
    /* 003C */ float unk_003C;
    /* 0040 */ float unk_0040;
    /* 0044 */ float unk_0044;
    /* 0048 */ float unk_0048;
    /* 004C */ float unk_004C;
    /* 0050 */ float unk_0050;
    /* 0054 */ float unk_0054;
    /* 0058 */ CM2Light m_light;
    /* 00C4 */ float unk_00C4;
    /* 00C8 */ float unk_00C8;
    /* 00CC */ float unk_00CC;
    /* 00D0 */ uint8_t unk_00D0;
    /* 00D1 */ uint8_t unk_00D1;
    /* 00D2 */ uint8_t unk_00D2;
    /* 00D3 */ uint8_t unk_00D3;
};

class CMapCacheLight {
    public:
    /* 0000 */ uint8_t unk_0000[132];
};

#endif
