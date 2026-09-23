#ifndef _TRIAGE_CPLANEPARTICLEEMITTER_H
#define _TRIAGE_CPLANEPARTICLEEMITTER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem2.h"

class CPlaneParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0234 */ float m_width;
    /* 0238 */ float m_height;
    /* 023C */ float m_latitude;
    /* 0240 */ float m_longitude;
};

#endif
