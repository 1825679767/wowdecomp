#ifndef WOW_SOURCE_UI_WORLDFRAME_H
#define WOW_SOURCE_UI_WORLDFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Event/Types.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/CRect.h"

struct CGCamera;

class CGWorldFrameUnkObjSomeElement {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ float unk_0014;
    /* 0018 */ float unk_0018;
    /* 001C */ float unk_001C;
    /* 0020 */ float unk_0020;
    /* 0024 */ float unk_0024;
    /* 0028 */ float unk_0028;
    /* 002C */ float unk_002C;
    /* 0030 */ float unk_0030;
    /* 0034 */ float unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ uint32_t unk_0054;
    /* 0058 */ uint32_t unk_0058;
    /* 005C */ uint32_t unk_005C;
    /* 0060 */ uint32_t unk_0060;
    /* 0064 */ uint32_t unk_0064;
};

class CGWorldFrameUnkObj {
    public:
    /* 0000 */ CGWorldFrameUnkObjSomeElement m_elements [16];
    /* 0680 */ uint8_t unk_0680 [202];
    /* 074A */ uint8_t unk_074A[2];
    /* 074C */ float m_tail [13];
};

struct HITTESTRESULT {
    /* 0000 */ WGUID m_guid;
    /* 0008 */ C3Vector m_point;
    /* 0014 */ float m_unused;
    /* 0018 */ C3Vector m_segStart;
    /* 0024 */ C3Vector m_segEnd;
};

struct KEYDOWNSTATE {
    /* 0000 */ char m_keyString [32];
    /* 0020 */ uint32_t m_modifiers;
};

class CGWorldFrame : public CSimpleFrame {
    public:
    /* 029C */ TSList<void> m_modelList;
    /* 02A8 */ TSList<void> m_hitModelList;
    /* 02B4 */ TSList<void> m_freeModelList;
    /* 02C0 */ uint32_t unk_02C0;
    /* 02C4 */ uint32_t unk_02C4;
    /* 02C8 */ WGUID m_trackedGuid;
    /* 02D0 */ WGUID m_hitGuid;
    /* 02D8 */ uint32_t m_defaultActionHitKind;
    /* 02DC */ uint32_t unk_02DC;
    /* 02E0 */ HITTESTRESULT m_defaultActionHit;
    /* 0310 */ C2Vector m_defaultActionPointDDC;
    /* 0318 */ uint32_t unk_0318;
    /* 031C */ uint32_t m_worldFlags;
    /* 0320 */ CRect m_viewportDDC;
    /* 0330 */ CRect m_viewportNDC;
    /* 0340 */ C44Matrix m_viewProjection;
    /* 0380 */ WGUID m_trackedEffectGuidA;
    /* 0388 */ WGUID m_trackedEffectGuidB;
    /* 0390 */ CGWorldFrameUnkObj unk_0390;
    /* 0B10 */ uint32_t m_namePlateFlags;
    /* 0B14 */ uint32_t m_elapsedSec;
    /* 0B18 */ KEYDOWNSTATE m_keyDown [787];
    /* 79C4 */ uint32_t m_mouseDown [279];
    /* 7E20 */ CGCamera* m_camera;
    /* 7E24 */ uint32_t unk_7E24;
};

class CWorldClickEvent {
    public:
    /* 0000 */ C3Vector m_segStart;
    /* 000C */ C3Vector m_segEnd;
    /* 0018 */ MOUSEBUTTON m_button;
};

#endif
