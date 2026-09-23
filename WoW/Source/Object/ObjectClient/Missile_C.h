#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_MISSILE_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"

class CM2Model;
class CMissile;

class CMissile {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ CM2Model* m_uknModel;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ C3Vector m_somePos;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t m_flags;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint32_t unk_0074;
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ uint32_t m_missileSound;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ uint32_t m_syncObject;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ float unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ uint32_t unk_00B8;
    /* 00BC */ uint32_t unk_00BC;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ uint32_t unk_00DC;
    /* 00E0 */ uint32_t unk_00E0;
    /* 00E4 */ uint32_t unk_00E4;
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ uint32_t unk_00F4;
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ uint32_t unk_00FC;
    /* 0100 */ uint32_t unk_0100;
    /* 0104 */ uint32_t unk_0104;
    /* 0108 */ uint32_t unk_0108;
    /* 010C */ uint32_t unk_010C;
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ uint32_t unk_0114;
    /* 0118 */ uint32_t unk_0118;
    /* 011C */ uint32_t m_speed;
    /* 0120 */ C3Vector m_uknPos2;
    /* 012C */ uint32_t m_travelTime;
    /* 0130 */ uint32_t unk_0130;
    /* 0134 */ uint32_t unk_0134;
    /* 0138 */ uint32_t unk_0138;
    /* 013C */ uint32_t unk_013C;
    /* 0140 */ uint32_t m_gravity;
    /* 0144 */ uint32_t unk_0144;
    /* 0148 */ uint32_t m_missileCollision;
    /* 014C */ uint32_t unk_014C;
    /* 0150 */ uint32_t unk_0150;
    /* 0154 */ uint32_t unk_0154;
    /* 0158 */ uint32_t unk_0158;
    /* 015C */ uint32_t unk_015C;
    /* 0160 */ CMissile* m_prevMaybe;
    /* 0164 */ CMissile* m_nextMaybe;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint32_t unk_016C;
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint32_t unk_0178;
    /* 017C */ uint32_t unk_017C;
    /* 0180 */ uint32_t unk_0180;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ uint32_t unk_0188;
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint32_t unk_0190;
    /* 0194 */ uint32_t unk_0194;
    /* 0198 */ uint32_t unk_0198;
    /* 019C */ uint32_t unk_019C;
    /* 01A0 */ uint32_t unk_01A0;
    /* 01A4 */ uint32_t unk_01A4;
    /* 01A8 */ uint32_t unk_01A8;
    /* 01AC */ uint32_t unk_01AC;
    /* 01B0 */ uint32_t unk_01B0;
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ uint32_t unk_01B8;
    /* 01BC */ uint32_t unk_01BC;
    /* 01C0 */ uint32_t unk_01C0;
    /* 01C4 */ uint32_t unk_01C4;
};

#endif
