#ifndef _TRIAGE_CAMERA_H
#define _TRIAGE_CAMERA_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/c34matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/CSimpleCamera.h"
#include "_Triage/CAngle.h"

class C3Vector;
class CM2Model;

class CAMERA_DEFAULTS {
    public:
    /* 0000 */ char* m_distance;
    /* 0004 */ char* m_pitch;
    /* 0008 */ char* unk_0008;
};

class CAMERA_SMOOTH {
    public:
    /* 0000 */ int32_t m_startTimeMs;
    /* 0004 */ float m_rate;
    /* 0008 */ float m_target;
    /* 000C */ float m_startValue;
    /* 0010 */ float m_param3;
    /* 0014 */ float m_param4;
};

class CCamera {
    public:
    /* 0000 */ TManaged<C3Vector> m_position;
    /* 0028 */ TManaged<C3Vector> m_target;
    /* 0050 */ TManaged<float> m_distance;
    /* 0070 */ TManaged<float> m_zFar;
    /* 0090 */ TManaged<float> m_zNear;
    /* 00B0 */ CAngle m_aoa;
    /* 00D8 */ CAngle m_fov;
    /* 0100 */ CAngle m_roll;
    /* 0128 */ CAngle m_rotation;
};

class CGCamera : public CSimpleCamera {
    public:
    /* 0048 */ CM2Model* m_model;
    /* 004C */ uint32_t m_someTime;
    /* 0050 */ uint32_t unk_0050;
    /* 0054 */ C34Matrix m_modelMatrix;
    /* 0084 */ uint32_t unk_0084;
    /* 0088 */ WGUID m_targetGUID;
    /* 0090 */ WGUID m_guid0090;
    /* 0098 */ uint32_t m_state;
    /* 009C */ uint32_t m_flags;
    /* 00A0 */ WGUID m_relativeTo;
    /* 00A8 */ uint32_t unk_00A8;
    /* 00AC */ uint32_t m_ignoreFacingRefs;
    /* 00B0 */ uint32_t unk_00B0;
    /* 00B4 */ uint32_t m_viewIndex;
    /* 00B8 */ uint32_t m_views [24];
    /* 0118 */ float m_distance;
    /* 011C */ float m_yaw;
    /* 0120 */ float m_pitch;
    /* 0124 */ float m_roll;
    /* 0128 */ float m_height;
    /* 012C */ float m_yawOffset;
    /* 0130 */ float m_targetOffset;
    /* 0134 */ uint32_t m_groundTilt;
    /* 0138 */ uint32_t m_targetFov;
    /* 013C */ float m_flyingMountHeight;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint32_t unk_0144;
    /* 0148 */ uint32_t unk_0148;
    /* 014C */ float m_targetHeightNear;
    /* 0150 */ float m_targetHeightFar;
    /* 0154 */ float m_targetHeightSwim;
    /* 0158 */ float m_mountHeight;
    /* 015C */ uint32_t m_heightChangedTimeMs;
    /* 0160 */ uint32_t m_motionFlags;
    /* 0164 */ uint32_t m_motionTime;
    /* 0168 */ uint32_t unk_0168;
    /* 016C */ uint32_t unk_016C;
    /* 0170 */ uint32_t unk_0170;
    /* 0174 */ uint32_t unk_0174;
    /* 0178 */ uint32_t unk_0178;
    /* 017C */ uint32_t m_motionEndTime;
    /* 0180 */ uint32_t m_lastZoomTime;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ uint32_t unk_0188;
    /* 018C */ uint32_t unk_018C;
    /* 0190 */ uint32_t unk_0190;
    /* 0194 */ uint32_t m_motionDeadline;
    /* 0198 */ uint32_t unk_0198;
    /* 019C */ uint32_t unk_019C;
    /* 01A0 */ uint32_t unk_01A0;
    /* 01A4 */ uint32_t unk_01A4;
    /* 01A8 */ uint32_t unk_01A8;
    /* 01AC */ uint32_t m_motionSpeed;
    /* 01B0 */ uint32_t unk_01B0;
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ uint32_t unk_01B8;
    /* 01BC */ uint32_t unk_01BC;
    /* 01C0 */ uint32_t unk_01C0;
    /* 01C4 */ C3Vector m_targetPosition;
    /* 01D0 */ uint32_t unk_01D0;
    /* 01D4 */ uint32_t m_targetFacing;
    /* 01D8 */ uint32_t unk_01D8;
    /* 01DC */ uint32_t unk_01DC;
    /* 01E0 */ CAMERA_SMOOTH m_smoothDistance;
    /* 01F8 */ CAMERA_SMOOTH m_smoothGroundTilt;
    /* 0210 */ CAMERA_SMOOTH m_smoothHeight;
    /* 0228 */ CAMERA_SMOOTH m_smoothPitch;
    /* 0240 */ CAMERA_SMOOTH m_smoothTargetOffset;
    /* 0258 */ CAMERA_SMOOTH m_smoothYaw;
    /* 0270 */ CAMERA_SMOOTH m_smoothFoV;
    /* 0288 */ uint32_t unk_0288;
    /* 028C */ uint32_t unk_028C;
    /* 0290 */ uint32_t unk_0290;
    /* 0294 */ uint32_t unk_0294;
    /* 0298 */ uint32_t unk_0298;
    /* 029C */ uint32_t unk_029C;
    /* 02A0 */ uint32_t unk_02A0;
    /* 02A4 */ uint32_t m_wasFlying;
    /* 02A8 */ CAMERA_SMOOTH m_smoothFlyingHeight;
    /* 02C0 */ uint32_t m_collideExtent;
    /* 02C4 */ uint32_t m_vehicleZoomEnabled;
    /* 02C8 */ uint32_t m_overrideDistanceMin;
    /* 02CC */ uint32_t m_overrideDistanceMax;
    /* 02D0 */ uint32_t unk_02D0;
    /* 02D4 */ uint32_t unk_02D4;
    /* 02D8 */ uint32_t unk_02D8;
    /* 02DC */ uint32_t unk_02DC;
    /* 02E0 */ uint32_t unk_02E0;
    /* 02E4 */ uint32_t unk_02E4;
    /* 02E8 */ uint32_t unk_02E8;
    /* 02EC */ uint32_t unk_02EC;
    /* 02F0 */ uint32_t unk_02F0;
    /* 02F4 */ uint32_t unk_02F4;
    /* 02F8 */ uint32_t unk_02F8;
    /* 02FC */ uint32_t unk_02FC;
    /* 0300 */ TSList<void> m_cameraShakeList;
    /* 030C */ uint32_t unk_030C;
    /* 0310 */ uint32_t unk_0310;
    /* 0314 */ uint32_t unk_0314;
    /* 0318 */ uint32_t unk_0318;
    /* 031C */ uint32_t m_vehicleCamera;
};

#endif
