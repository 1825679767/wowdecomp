#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_GAMEOBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_GAMEOBJECT_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/Passenger.h"

class CGGameObject_C : public CGObject_C, public CGGameObject {
    public:
    /* 00D8 */ CPassenger m_passenger;
    /* 0108 */ TSList<void> m_gameObjectMirrorLists [6];
    /* 0150 */ uint32_t unk_0150;
    /* 0154 */ uint32_t unk_0154;
    /* 0158 */ uint32_t unk_0158;
    /* 015C */ uint32_t unk_015C;
    /* 0160 */ uint32_t unk_0160;
    /* 0164 */ uint32_t unk_0164;
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
    /* 01A8 */ C44Matrix m_matrix;
    /* 01E8 */ uint32_t unk_01E8;
    /* 01EC */ uint32_t unk_01EC;
    /* 01F0 */ uint32_t unk_01F0;
    /* 01F4 */ uint32_t unk_01F4;
    /* 01F8 */ uint32_t unk_01F8;
    /* 01FC */ uint32_t unk_01FC;
    /* 0200 */ uint32_t unk_0200;
    /* 0204 */ uint32_t unk_0204;
    /* 0208 */ uint32_t unk_0208;
    /* 020C */ uint32_t unk_020C;
    /* 0210 */ uint32_t unk_0210;
    /* 0214 */ uint32_t unk_0214;
};

class CGGameObject_C_TypeAnimated {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint8_t unk_0004;
    /* 0005 */ uint8_t unk_0005;
    /* 0006 */ uint8_t unk_0006;
    /* 0007 */ uint8_t unk_0007;
    /* 0008 */ uint16_t unk_0008;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B[4];
    /* 000F */ uint8_t unk_000F[1];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
};

class CGGameObject_C_TypeBase {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
};

class CGGameObject_C_Type_DestructibleBuilding {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint16_t unk_001E;
    /* 0020 */ uint8_t unk_0020;
    /* 0021 */ uint8_t unk_0021[2];
    /* 0023 */ uint8_t unk_0023;
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ uint16_t unk_0028;
    /* 002A */ uint8_t unk_002A[4];
    /* 002E */ uint8_t unk_002E[10];
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint8_t unk_003C[12];
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint8_t unk_004C[8];
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ uint32_t unk_0068;
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[32];
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ uint32_t unk_0098;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint8_t unk_00AC;
    /* 00AD */ uint8_t unk_00AD;
};

class CGGameObject_C_Type_FishingNode {
    public:
    /* 0000 */ uint8_t unk_0000[3];
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[40];
};

class CGGameObject_C_Type_GuildBank {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
};

class CGGameObject_C_Type_MapObjTransport {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint8_t unk_0010;
    /* 0011 */ uint8_t unk_0011;
    /* 0012 */ uint8_t unk_0012;
    /* 0013 */ uint8_t unk_0013;
    /* 0014 */ uint8_t unk_0014;
    /* 0015 */ uint8_t unk_0015;
    /* 0016 */ uint8_t unk_0016;
    /* 0017 */ uint8_t unk_0017;
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019;
    /* 001A */ uint8_t unk_001A;
    /* 001B */ uint8_t unk_001B;
    /* 001C */ uint8_t unk_001C;
    /* 001D */ uint8_t unk_001D;
    /* 001E */ uint16_t unk_001E;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint8_t unk_002C[4];
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint8_t unk_0040[60];
    /* 007C */ uint8_t unk_007C;
    /* 007D */ uint8_t unk_007D;
    /* 007E */ uint8_t unk_007E;
    /* 007F */ uint8_t unk_007F[1];
    /* 0080 */ uint32_t unk_0080;
};

class CGGameObject_C_Type_MeetingStone {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
};

class CGGameObject_C_Type_Ritual {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[44];
};

class CGGameObject_C_Type_TransportBase {
    public:
    /* 0000 */ uint8_t unk_0000;
    /* 0001 */ uint8_t unk_0001;
    /* 0002 */ uint8_t unk_0002;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint8_t unk_0004;
    /* 0005 */ uint8_t unk_0005;
    /* 0006 */ uint8_t unk_0006;
    /* 0007 */ uint8_t unk_0007;
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E;
    /* 000F */ uint8_t unk_000F;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[4];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint8_t unk_0030[4];
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
};

class CGGameObject_C_Type_WorldObj {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
};

#endif
