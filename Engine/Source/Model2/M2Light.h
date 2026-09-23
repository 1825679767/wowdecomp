#ifndef ENGINE_SOURCE_MODEL2_M2LIGHT_H
#define ENGINE_SOURCE_MODEL2_M2LIGHT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/c4plane.h"
#include "Engine/Source/Tempest/caasphere.h"

class CM2Light;
struct CM2Scene;

class CM2Light {
    public:
    /* 0000 */ CM2Scene* m_scene;
    /* 0004 */ uint32_t m_stamp;
    /* 0008 */ uint32_t m_type;
    /* 000C */ C3Vector m_pos;
    /* 0018 */ C3Vector m_viewPos;
    /* 0024 */ C3Vector m_dir;
    /* 0030 */ C3Vector m_ambColor;
    /* 003C */ C3Vector m_dirColor;
    /* 0048 */ C3Vector m_specColor;
    /* 0054 */ float m_constantAttenuation;
    /* 0058 */ float m_linearAttenuation;
    /* 005C */ float m_quadraticAttenuation;
    /* 0060 */ uint32_t m_visible;
    /* 0064 */ CM2Light** m_lightPrev;
    /* 0068 */ CM2Light* m_lightNext;
};

class CM2Lighting {
    public:
    /* 0000 */ CM2Scene* m_scene;
    /* 0004 */ CAaSphere m_sphere4;
    /* 0014 */ uint32_t m_flags;
    /* 0018 */ C3Vector m_vector18;
    /* 0024 */ C3Vector m_vector24;
    /* 0030 */ C3Vector m_vector30;
    /* 003C */ C3Vector m_vector3C;
    /* 0048 */ C3Vector m_vector48;
    /* 0054 */ C3Vector m_sunAmbient;
    /* 0060 */ C3Vector m_sunDiffuse;
    /* 006C */ C3Vector m_sunSpecular;
    /* 0078 */ C3Vector m_sunDir;
    /* 0084 */ CM2Light* m_lights [4];
    /* 0094 */ float m_lightDist [4];
    /* 00A4 */ uint32_t m_lightCount;
    /* 00A8 */ float m_fogStart;
    /* 00AC */ float m_fogEnd;
    /* 00B0 */ float m_fogScale;
    /* 00B4 */ float m_fogDensity;
    /* 00B8 */ C3Vector m_fogColor;
    /* 00C4 */ C4Plane m_liquidPlane;
};

#endif
