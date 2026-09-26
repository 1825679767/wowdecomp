#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMODEL_H
#define ENGINE_SOURCE_FRAME_CSIMPLEMODEL_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Tempest/CImVector.h"

class CM2Model;
class CM2Scene;

class CSimpleModel : public CSimpleFrame {
    public:
    /* 029C */ CM2Scene* m_scene;
    /* 02A0 */ CM2Model* m_model;
    /* 02A4 */ uint32_t m_camera;
    /* 02A8 */ uint32_t m_pendingCameraIndex;
    /* 02AC */ uint32_t unk_02AC;
    /* 02B0 */ CM2Light m_m2Light;
    /* 031C */ float unk_031C;
    /* 0320 */ float unk_0320;
    /* 0324 */ float unk_0324;
    /* 0328 */ float unk_0328;
    /* 032C */ float m_modelScale;
    /* 0330 */ uint32_t m_flags;
    /* 0334 */ CImVector m_fogColor;
    /* 0338 */ float m_fogNear;
    /* 033C */ float m_fogFar;
    /* 0340 */ float unk_0340;
    /* 0344 */ float unk_0344;
    /* 0348 */ float unk_0348;
    /* 034C */ float unk_034C;
    /* 0350 */ float unk_0350;
    /* 0354 */ float unk_0354;
    /* 0358 */ FrameScript_Object::ScriptIx m_onUpdateModel;
    /* 0360 */ FrameScript_Object::ScriptIx m_onAnimFinished;

    static CDataAllocator s_allocator;

    CSimpleModel(CSimpleFrame* parent);
};

#endif
