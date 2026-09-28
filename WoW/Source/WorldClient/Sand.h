#ifndef WOW_SOURCE_WORLDCLIENT_SAND_H
#define WOW_SOURCE_WORLDCLIENT_SAND_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/Rain.h"

class CGxShader;
struct SandGrainParticleBuffer;

struct SandGrainParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ C3Vector m_velocity;
    /* 0018 */ float m_lifetime;
    /* 001C */ float m_size;
};

struct SandGrainParticleBuffer {
    /* 0000 */ uint32_t m_particleCount;
    /* 0004 */ SandGrainParticle m_particles [6144];
};

class Sand {
    public:
    /* 0000 */ int32_t m_useWeatherShaders;
    /* 0004 */ CAaBox m_bbox;
    /* 001C */ Mists m_mists;
    /* 0070 */ PacketList<Packet<SandGrainParticleBuffer>> m_sandGrainPacketList;
    /* 0090 */ Packet<SandGrainParticleBuffer>* m_sandGrainPacketPtr;
    /* 0094 */ float unk_0094;
    /* 0098 */ CGxShader* m_shader;
    /* 009C */ float unk_009C;

    ~Sand();
};

#endif
