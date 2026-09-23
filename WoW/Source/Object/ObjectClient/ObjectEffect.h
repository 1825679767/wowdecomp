#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECTEFFECT_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECTEFFECT_H

#include <cstddef>
#include <cstdint>

class CObjectEffect {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[4];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
};

class CObjectEffectManager {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[12];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[84];
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[4];
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
};

#endif
