#ifndef ENGINE_SOURCE_GX_CGXLIGHT_H
#define ENGINE_SOURCE_GX_CGXLIGHT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Tempest/C3Vector.h"

class CGxLight {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_dir;
    /* 0010 */ C3Vector m_ambientColor;
    /* 001C */ C3Vector m_dirColor;
    /* 0028 */ C3Vector m_specularColor;
    /* 0034 */ float m_constantAttenuation;
    /* 0038 */ float m_linearAttenuation;
    /* 003C */ float m_quadraticAttenuation;
};

#endif
