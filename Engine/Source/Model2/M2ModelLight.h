#ifndef ENGINE_SOURCE_MODEL2_M2MODELLIGHT_H
#define ENGINE_SOURCE_MODEL2_M2MODELLIGHT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Tempest/C3Vector.h"

struct C3Vector;

class M2ModelLight {
    public:
    /* 0000 */ M2ModelTrack<C3Vector> m_ambientColorTrack;
    /* 0014 */ M2ModelTrack<float> m_ambientIntensityTrack;
    /* 0020 */ M2ModelTrack<C3Vector> m_diffuseColorTrack;
    /* 0034 */ M2ModelTrack<float> m_diffuseIntensityTrack;
    /* 0040 */ M2ModelTrack<uint8_t> m_visibilityTrack;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
    /* 0068 */ CM2Light m_light;
};

#endif
