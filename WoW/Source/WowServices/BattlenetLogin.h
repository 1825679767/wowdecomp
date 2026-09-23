#ifndef WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H
#define WOW_SOURCE_WOWSERVICES_BATTLENETLOGIN_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/WowServices/Login.h"

class BattlenetLogin : public Login {
    public:
    /* 10F0 */ int32_t m_serverId;
    /* 10F4 */ uint32_t unk_10F4;
    /* 10F8 */ uint32_t unk_10F8;
    /* 10FC */ uint32_t unk_10FC;
    /* 1100 */ uint32_t unk_1100;
    /* 1104 */ uint32_t unk_1104;
    /* 1108 */ uint8_t m_ukn1061;
    /* 1109 */ uint8_t m_ukn1062;
    /* 110A */ uint8_t m_ukn1063;
    /* 110B */ uint8_t m_ukn1064;
    /* 110C */ uint8_t m_ukn1071;
    /* 110D */ uint8_t m_ukn1072;
    /* 110E */ uint8_t m_ukn1073;
    /* 110F */ uint8_t m_ukn1074;
    /* 1110 */ uint8_t m_ukn1081;
    /* 1111 */ uint8_t m_ukn1082;
    /* 1112 */ uint8_t m_ukn1083;
    /* 1113 */ uint8_t m_ukn1084;
    /* 1114 */ uint32_t unk_1114;
    /* 1118 */ uint32_t unk_1118;
    /* 111C */ uint32_t unk_111C;
    /* 1120 */ uint32_t unk_1120;
    /* 1124 */ uint32_t unk_1124;
    /* 1128 */ uint32_t unk_1128;
    /* 112C */ uint32_t unk_112C;
    /* 1130 */ uint32_t unk_1130;
    /* 1134 */ uint32_t unk_1134;
    /* 1138 */ uint32_t unk_1138;
    /* 113C */ uint8_t m_ukn1191;
    /* 113D */ uint8_t m_ukn1192;
    /* 113E */ uint8_t m_ukn1193;
    /* 113F */ uint8_t m_gruntMatrixRequired;
    /* 1140 */ uint8_t m_gruntMatrixWidth;
    /* 1141 */ uint8_t m_gruntMatrixHeight;
    /* 1142 */ uint8_t m_ukn1203;
    /* 1143 */ uint8_t m_gruntMatrixChallengeCount;
    /* 1144 */ uint32_t unk_1144;
    /* 1148 */ uint32_t unk_1148;
    /* 114C */ uint32_t unk_114C;
    /* 1150 */ uint32_t unk_1150;
    /* 1154 */ uint32_t unk_1154;
    /* 1158 */ uint32_t unk_1158;
    /* 115C */ uint32_t unk_115C;
    /* 1160 */ uint32_t m_matrixClientProof;
    /* 1164 */ uint32_t unk_1164;
    /* 1168 */ uint32_t unk_1168;
    /* 116C */ uint32_t unk_116C;
    /* 1170 */ uint32_t unk_1170;
    /* 1174 */ uint8_t m_tokenInfoRequired;
    /* 1175 */ uint8_t m_tokenInfoUkn2;
    /* 1176 */ uint8_t m_tokenInfoString;
    /* 1177 */ uint8_t m_ukn1334;
    /* 1178 */ uint32_t unk_1178;
    /* 117C */ uint32_t unk_117C;
    /* 1180 */ uint32_t unk_1180;
    /* 1184 */ uint32_t unk_1184;
    /* 1188 */ uint32_t m_battleUkn1;
    /* 118C */ uint32_t m_battleUkn2;
    /* 1190 */ uint32_t m_battleUkn3;
    /* 1194 */ uint32_t m_battleUkn4;
    /* 1198 */ uint32_t m_battleUkn5;
    /* 119C */ uint32_t m_battleUkn6;
    /* 11A0 */ uint32_t m_battleUkn7;
    /* 11A4 */ uint32_t m_battleUkn8;
    /* 11A8 */ uint32_t m_battleUkn9;
    /* 11AC */ uint32_t m_battleUkn10;
    /* 11B0 */ uint32_t m_battleUkn11;
    /* 11B4 */ uint32_t m_battleUkn12;
    /* 11B8 */ uint32_t m_accountFlags;
    /* 11BC */ uint32_t m_battleUkn14;
    /* 11C0 */ uint32_t m_battleUkn15;
    /* 11C4 */ uint32_t m_battleUkn16;
    /* 11C8 */ uint32_t m_battleUkn17;
};

class BNetAuthPacketSub1 {
    public:
    /* 0000 */ uint32_t unk_0000;
};

class BattlenetApplication {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint8_t unk_0008;
    /* 0009 */ uint8_t unk_0009;
    /* 000A */ uint8_t unk_000A;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D;
    /* 000E */ uint8_t unk_000E[4];
    /* 0012 */ uint16_t unk_0012;
    /* 0014 */ uint16_t unk_0014;
    /* 0016 */ uint16_t unk_0016;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
};

#endif
