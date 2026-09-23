#ifndef WOW_SOURCE_OBJECT_GAMEOBJECTSTATS_H
#define WOW_SOURCE_OBJECT_GAMEOBJECTSTATS_H

#include <cstddef>
#include <cstdint>

class GameObjectStats {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[96];
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint8_t unk_0078[24];
    /* 0090 */ uint32_t unk_0090;
};

#endif
