#ifndef WOW_SOURCE_WORLDCLIENT_RAIN_H
#define WOW_SOURCE_WORLDCLIENT_RAIN_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/caabox.h"
#include "Engine/Source/Tempest/cimvector.h"
#include "WoW/Source/WorldClient/Mists.h"

class CGxShader;
class CTexture;
struct RainParticleBuffer;
struct RainPatterParticleBuffer;
struct CGxPool;
struct CGxBuf;

struct RainDropParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_velocity;
    /* 0018 */ CImVector m_color;
    /* 001C */ float m_size;
    /* 0020 */ float unk_0020;
};

struct RainParticleBuffer {
    /* 0000 */ uint32_t m_particleCount;
    /* 0004 */ RainDropParticle m_particles [6144];
};

struct RainPatterParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ float m_lifetime;
    /* 0010 */ float m_size;
    /* 0014 */ float unk_0014;
};

struct RainPatterParticleBuffer {
    /* 0000 */ uint32_t m_particleCount;
    /* 0004 */ RainPatterParticle m_particles [6144];
};

template <class T>
struct PacketList {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ T* m_prev;
    /* 0008 */ T* m_next;
    /* 000C */ int32_t m_activePacketsCount;
    /* 0010 */ int32_t m_freePacketsCount;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ T* unk_0018;
    /* 001C */ T* unk_001C;
};

template <class T>
struct Packet {
    /* 0000 */ Packet<T>* m_prev;
    /* 0004 */ Packet<T>* m_next;
    /* 0008 */ T m_particlesBuffer;
    /* 3600C */ uint32_t m_currParticle;
    /* 36010 */ uint32_t m_packetStartTick;
    /* 36014 */ uint32_t m_packetEndTick;
    /* 36018 */ float m_packetBuildTime;
    /* 3601C */ uint32_t unk_3601C;
    /* 36020 */ uint32_t unk_36020;
    /* 36024 */ uint32_t unk_36024;
    /* 36028 */ CGxPool* m_gxPool;
    /* 3602C */ CGxBuf* m_vertexBufHandle;
};

class Rain {
    public:
    /* 0000 */ int32_t m_useWeatherShaders;
    /* 0004 */ CAaBox m_bbox;
    /* 001C */ Mists m_mists;
    /* 0070 */ PacketList<Packet<RainParticleBuffer>> m_rainDropPacketList;
    /* 0090 */ PacketList<Packet<RainPatterParticleBuffer>> m_rainPatterPacketList;
    /* 00B0 */ Packet<RainParticleBuffer>* m_rainDropPacketPtr;
    /* 00B4 */ float unk_00B4;
    /* 00B8 */ Packet<RainPatterParticleBuffer>* m_rainPatterPacketPtr;
    /* 00BC */ CTexture* m_rainDropTex;
    /* 00C0 */ CGxShader* m_shaderRain;
    /* 00C4 */ CTexture* m_rainDropSplashTex;
    /* 00C8 */ CGxShader* m_shaderPatter;
    /* 00CC */ float m_particlesPerFrame;
    /* 00D0 */ float m_intensity;
};

#endif
