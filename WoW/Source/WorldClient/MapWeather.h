#ifndef WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H
#define WOW_SOURCE_WORLDCLIENT_MAPWEATHER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/caabox.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/Rain.h"

class CGxShader;
class CTexture;
struct SnowFlakeParticleBuffer;

class Snow {
    public:
    /* 0000 */ int32_t m_useWeatherShaders;
    /* 0004 */ CAaBox m_bbox;
    /* 001C */ Mists m_mists;
    /* 0070 */ int32_t unk_0070;
    /* 0074 */ void* unk_0074;
    /* 0078 */ void* unk_0078;
    /* 007C */ int32_t m_activePacketsCount;
    /* 0080 */ int32_t m_freePacketsCount;
    /* 0084 */ int32_t unk_0084;
    /* 0088 */ void* unk_0088;
    /* 008C */ void* unk_008C;
    /* 0090 */ Packet<SnowFlakeParticleBuffer>* m_snowFlakePacketPtr;
    /* 0094 */ float unk_0094;
    /* 0098 */ CTexture* m_snowFlakeTex;
    /* 009C */ CGxShader* m_shaderPtr;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ int32_t unk_00A4;
};

struct SnowFlakeParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_velocity;
    /* 0018 */ float m_lifetime;
    /* 001C */ float m_size;
};

struct SnowFlakeParticleBuffer {
    /* 0000 */ uint32_t m_particleCount;
    /* 0004 */ SnowFlakeParticle m_particles [6144];
};

class Weather {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C [99];
};

class MapWeather {
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
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026[2];
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint8_t unk_0034[172];
    /* 00E0 */ uint8_t unk_00E0;
    /* 00E1 */ uint8_t unk_00E1[91];
    /* 013C */ uint32_t unk_013C;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint32_t unk_0144;
    /* 0148 */ uint32_t unk_0148;
    /* 014C */ uint8_t unk_014C[24];
    /* 0164 */ uint32_t unk_0164;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint8_t unk_016C[4];
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint8_t unk_0178[4];
    /* 017C */ uint32_t unk_017C;
    /* 0180 */ uint32_t unk_0180;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ uint8_t unk_0188;
    /* 0189 */ uint8_t unk_0189[3];
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint32_t unk_0190;
    /* 0194 */ uint8_t unk_0194;
    /* 0195 */ uint8_t unk_0195[3];
};

#endif
