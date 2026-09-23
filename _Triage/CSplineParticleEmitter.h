#ifndef _TRIAGE_CSPLINEPARTICLEEMITTER_H
#define _TRIAGE_CSPLINEPARTICLEEMITTER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"

class CSplineParticleEmitter : public CParticleEmitter2 {
    public:
    /* 0234 */ float m_ratePerUnit;
    /* 0238 */ float m_width;
    /* 023C */ float m_splineLength;
    /* 0240 */ float m_latitude;
    /* 0244 */ float m_longitude;
    /* 0248 */ float m_spawnAtEnd;
    /* 024C */ C3Spline_CatmullRom m_spline;
};

#endif
