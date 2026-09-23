#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_BAG_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_BAG_C_H

#include <cstddef>
#include <cstdint>

class CGBag {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[56];
    /* 0040 */ uint8_t unk_0040;
};

class CGBag_C {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011[47];
    /* 0040 */ uint8_t unk_0040;
};

#endif
