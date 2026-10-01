#ifndef WOW_SOURCE_GAME_SHIPPATH_H
#define WOW_SOURCE_GAME_SHIPPATH_H

#include <cstddef>
#include <cstdint>

class TransportPhysicsRec;

class ShipPath {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ float unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint8_t unk_003C;
    /* 003D */ uint8_t unk_003D;
    /* 003E */ uint8_t unk_003E;

    ShipPath();
    void WaitAtNextPausePoint(uint32_t time, bool pause);
    void Init(uint32_t taxiPathId, float moveSpeed, float accelRate, const TransportPhysicsRec* physics);
    void SetTotalPathTime(uint32_t time);
};

#endif
