#ifndef ENGINE_SOURCE_COMSAT_TYPES_H
#define ENGINE_SOURCE_COMSAT_TYPES_H

#include <cstddef>
#include <cstdint>

namespace ComSat {
namespace Sound {
struct IOSoundEngine {
    /* 0000 */ uint8_t unk_0000 [8];
    /* 0008 */ int32_t unk_0008;
    /* 000C */ uint8_t unk_000C[4];
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ uint8_t unk_0018 [4];
    /* 001C */ int32_t unk_001C;
    /* 0020 */ uint8_t unk_0020 [4];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ uint8_t unk_0030 [32804];
    /* 8054 */ int32_t unk_8054;
    /* 8058 */ int32_t unk_8058;
    /* 805C */ int32_t unk_805C;
    /* 8060 */ int32_t unk_8060;
    /* 8064 */ int32_t unk_8064;
};
}
}

#endif
