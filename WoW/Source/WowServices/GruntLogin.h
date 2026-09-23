#ifndef WOW_SOURCE_WOWSERVICES_GRUNTLOGIN_H
#define WOW_SOURCE_WOWSERVICES_GRUNTLOGIN_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/WowServices/Login.h"

namespace Grunt { struct ClientLink; }

class GruntLogin : public Login {
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
    /* 1188 */ Grunt::ClientLink* m_clientLink;
};

#endif
