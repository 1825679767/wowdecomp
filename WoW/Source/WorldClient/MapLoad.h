#ifndef WOW_SOURCE_WORLDCLIENT_MAPLOAD_H
#define WOW_SOURCE_WORLDCLIENT_MAPLOAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/caabox.h"
#include "Engine/Source/Tempest/caasphere.h"
#include "Engine/Source/Tempest/cimvector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Model;

class CMapStaticEntity : public CMapBaseObj {
    public:
    /* 0024 */ uint8_t unk_0024;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ uint32_t m_unkFlags28;
    /* 002C */ int32_t m_unkCounter;
    /* 0030 */ float m_distanceToCamera;
    /* 0034 */ CM2Model* m_model;
    /* 0038 */ CAaSphere m_sphere;
    /* 0048 */ CAaBox m_bbox;
    /* 0060 */ C3Vector m_vec2;
    /* 006C */ C3Vector m_position;
    /* 0078 */ float m_scale;
    /* 007C */ int32_t unk_007C;
    /* 0080 */ int32_t unk_0080;
    /* 0084 */ CImVector m_m2AmbietColor;
    /* 0088 */ CImVector m_m2DiffuseColor;
    /* 008C */ float unk_008C;
};

#endif
