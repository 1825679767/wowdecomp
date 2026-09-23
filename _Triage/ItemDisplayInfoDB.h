#ifndef _TRIAGE_ITEMDISPLAYINFODB_H
#define _TRIAGE_ITEMDISPLAYINFODB_H

#include <cstddef>
#include <cstdint>

class ItemDisplayInfoDB {
    public:
    /* 0000 */ uint8_t unk_0000[12];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[12];
    /* 0020 */ uint32_t unk_0020;
};

#endif
