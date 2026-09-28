#ifndef ENGINE_SOURCE_COMSAT_COMSATSOUNDIOSOUNDENGINE_H
#define ENGINE_SOURCE_COMSAT_COMSATSOUNDIOSOUNDENGINE_H

#include <cstddef>
#include <cstdint>

namespace ComSat {
struct Manager {
    /* 0000 */ uint8_t unk_0000 [8];
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
    /* 0010 */ uint8_t unk_0010 [4];
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ uint8_t unk_0018 [4];
    /* 001C */ int32_t unk_001C;
    /* 0020 */ int16_t unk_0020;
    /* 0022 */ uint8_t unk_0022[2];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ uint8_t unk_002C [4];
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ uint8_t unk_003C [44];
    /* 0068 */ int32_t unk_0068;
};
}

namespace ComSat {
struct Session {
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ uint8_t unk_0008 [8];
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
};
}

namespace ComSat {
namespace Sound {
struct Manager {
    /* 0004 */ uint8_t unk_0004 [8];
    /* 000C */ int32_t unk_000C;
    /* 0010 */ uint8_t unk_0010 [8];
    /* 0018 */ int32_t unk_0018;

    virtual ~Manager();
};
}
}

#endif
