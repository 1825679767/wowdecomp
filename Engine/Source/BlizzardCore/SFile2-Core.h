#ifndef ENGINE_SOURCE_BLIZZARDCORE_SFILE2_CORE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SFILE2_CORE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/Types.h"

class SEvent;

struct SOVERLAPPED {
    /* 0000 */ uint32_t Offset;
    /* 0004 */ SEvent* hEvent;
};

class SFile {
    public:
    static void STORMAPI CreateOverlapped(SOVERLAPPED* overlapped);
    static void Destroy();
    static void STORMAPI DestroyOverlapped(SOVERLAPPED* overlapped);
    static int32_t Initialize();

    /* 0000 */ uint8_t unk_0000[1];
};

class SFileImpl {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[4];
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
};

#endif
