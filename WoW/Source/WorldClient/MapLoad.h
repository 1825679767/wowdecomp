#ifndef WOW_SOURCE_WORLDCLIENT_MAPLOAD_H
#define WOW_SOURCE_WORLDCLIENT_MAPLOAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Lighting;
class CMapObjDef;

class CM2Model;

class CMapStaticEntity : public CMapBaseObj {
    public:
    static float s_characterAmbient;
    static C3Vector s_interiorSunDir;

    static void ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg);

    /* 0024 */ uint8_t m_fadeLevel;
    /* 0025 */ uint8_t unk_0025;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ uint8_t unk_0028;
    /* 0029 */ uint8_t unk_0029;
    /* 002A */ uint8_t unk_002A;
    /* 002B */ uint8_t unk_002B;
    /* 002C */ int32_t m_unkCounter;
    /* 0030 */ float m_distanceToCamera;
    /* 0034 */ CM2Model* m_model;
    /* 0038 */ CAaSphere m_sphere;
    /* 0048 */ CAaBox m_bbox;
    /* 0060 */ C3Vector m_vec2;
    /* 006C */ C3Vector m_position;
    /* 0078 */ float m_scale;
    /* 007C */ uint32_t unk_007C;
    /* 0080 */ float unk_0080;
    /* 0084 */ CImVector m_m2AmbietColor;
    /* 0088 */ CImVector m_m2DiffuseColor;
    /* 008C */ float m_diffuseLightScale;

    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);
    virtual bool QueryInteriorLighting(CMapObjDef* mapObjDef, int32_t groupNum, uint16_t* faceIndex, C3Vector* pos) = 0;

    CMapStaticEntity();
};

#endif
