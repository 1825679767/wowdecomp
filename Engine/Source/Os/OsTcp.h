#ifndef ENGINE_SOURCE_OS_OSTCP_H
#define ENGINE_SOURCE_OS_OSTCP_H

#include <cstddef>
#include <cstdint>

class OsNet {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint8_t unk_0004;
    /* 0005 */ uint8_t unk_0005[4];
    /* 0009 */ uint8_t unk_0009[5];
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012[5];
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint8_t unk_0018[3];
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D[4];
    /* 0021 */ uint8_t unk_0021[23];
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint8_t unk_0044[36];
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint8_t unk_0080[269];
    /* 018D */ uint8_t unk_018D;
    /* 018E */ uint8_t unk_018E;
    /* 018F */ uint8_t unk_018F;
    /* 0190 */ uint8_t unk_0190;
    /* 0191 */ uint8_t unk_0191;
    /* 0192 */ uint8_t unk_0192[4];
    /* 0196 */ uint8_t unk_0196[1146];
    /* 0610 */ uint32_t unk_0610;
};

void OsNetPump(uint32_t timeout);

#endif
