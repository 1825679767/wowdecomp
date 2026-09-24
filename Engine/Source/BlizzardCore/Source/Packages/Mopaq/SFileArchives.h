#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEARCHIVES_H

#include <cstddef>
#include <cstdint>

namespace System_SFile {
class ARCHIVEREC {
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
    /* 003C */ uint8_t unk_003C[8];
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint8_t unk_0048[4];
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint8_t unk_0050[36];
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint8_t unk_0078[12];
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ uint8_t unk_0088[284];
    /* 01A4 */ uint32_t unk_01A4;
    /* 01A8 */ uint32_t unk_01A8;
    /* 01AC */ uint8_t unk_01AC[4];
    /* 01B0 */ uint32_t unk_01B0;
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ uint8_t unk_01B8[8];
    /* 01C0 */ uint32_t unk_01C0;
    /* 01C4 */ uint32_t unk_01C4;
};
}

namespace System_SFile {
class BitPacked {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
};
}

namespace System_SFile {
class BlockEntryTable {
    public:
    /* 0000 */ uint8_t unk_0000[60];
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint8_t unk_004C[4];
    /* 0050 */ uint32_t unk_0050;
};
}

namespace System_SFile {
class HashEntryTable {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
};
}

#endif
