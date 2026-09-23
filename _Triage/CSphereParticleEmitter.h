#ifndef _TRIAGE_CSPHEREPARTICLEEMITTER_H
#define _TRIAGE_CSPHEREPARTICLEEMITTER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem2.h"

class CSphereParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0234 */ float m_minRadius;
    /* 0238 */ float m_maxRadius;
    /* 023C */ float m_radiusSpan;
    /* 0240 */ float m_longitude;
    /* 0244 */ float m_longitude2;
};

#endif
