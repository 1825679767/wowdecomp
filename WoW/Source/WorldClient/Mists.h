#ifndef WOW_SOURCE_WORLDCLIENT_MISTS_H
#define WOW_SOURCE_WORLDCLIENT_MISTS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CTexture;

class Mists {
    public:
    /* 0004 */ C3Vector m_position;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ float m_scale;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ float m_rotationSpeed;
    /* 0028 */ float m_fadeSpeed;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ float m_opacity;
    /* 0034 */ float m_lifeTime;
    /* 0038 */ float m_spawnRate;
    /* 003C */ uint8_t unk_003C;
    /* 003D */ uint8_t m_padding [3];
    /* 0040 */ CTexture* m_mistTexture;
    /* 0044 */ uint32_t m_particleCount;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;

    virtual ~Mists();
};

#endif
