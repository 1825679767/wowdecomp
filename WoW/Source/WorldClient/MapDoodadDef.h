#ifndef WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPDOODADDEF_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Model;

class CMapDoodadDef : public CMapBaseObj {
    public:
    /* 0024 */ uint8_t m_fadeLevel;
    /* 0025 */ uint8_t m_culled;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ uint32_t m_unkFlags28;
    /* 002C */ int32_t m_unkCounter;
    /* 0030 */ float m_distToCameraPlane;
    /* 0034 */ CM2Model* m_model;
    /* 0038 */ CAaSphere m_sphere;
    /* 0048 */ CAaBox m_bboxStaticEntity;
    /* 0060 */ C3Vector m_vec2;
    /* 006C */ C3Vector m_position;
    /* 0078 */ float m_scale;
    /* 007C */ int32_t unk_007C;
    /* 0080 */ int32_t unk_0080;
    /* 0084 */ CImVector m_m2AmbietColor;
    /* 0088 */ CImVector m_m2DiffuseColor;
    /* 008C */ float m_diffuseLightScale;
    /* 0090 */ uint32_t m_uniqueId;
    /* 0094 */ void* unk_0094;
    /* 0098 */ void* unk_0098;
    /* 009C */ void* m_prevMapDoodadDef09c;
    /* 00A0 */ void* m_nextMapDoodadDef0a0;
    /* 00A4 */ int32_t unk_00A4;
    /* 00A8 */ TSLink<void> m_doodadDefLink;
    /* 00B0 */ int32_t m_cullPassStamp;
    /* 00B4 */ int32_t unk_00B4;
    /* 00B8 */ int32_t unk_00B8;
    /* 00BC */ int32_t unk_00BC;
    /* 00C0 */ CAaBox m_bboxDoodadDef;
    /* 00D8 */ C44Matrix m_mat;
    /* 0118 */ C44Matrix m_identity;
    /* 0158 */ int32_t unk_0158;
    /* 015C */ int32_t unk_015C;
    /* 0160 */ int32_t unk_0160;
    /* 0164 */ int32_t unk_0164;
    /* 0168 */ int32_t unk_0168;
    /* 016C */ int32_t unk_016C;
};

#endif
