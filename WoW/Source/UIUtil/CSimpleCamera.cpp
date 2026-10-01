#include "CSimpleCamera.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"

// VERIFY: fbf6cda8 auto
// OFFSET: 0x005FEE70
float CSimpleCamera::FOV() const {
    return this->m_fov;
}

// VERIFY: 98278b28 auto
// OFFSET: 0x005FEE80
C3Vector CSimpleCamera::Forward() const {
    return C3Vector(this->m_facing.a0, this->m_facing.a1, this->m_facing.a2);
}

// VERIFY: 8b1f5f49 auto
// OFFSET: 0x005FEEA0
C3Vector CSimpleCamera::Right() const {
    return C3Vector(this->m_facing.b0, this->m_facing.b1, this->m_facing.b2);
}

// VERIFY: 8ec84a54 auto
// OFFSET: 0x005FEEC0
C3Vector CSimpleCamera::Up() const {
    return C3Vector(this->m_facing.c0, this->m_facing.c1, this->m_facing.c2);
}

// VERIFY: 24a464c0 manual
// OFFSET: 0x00607C20
CSimpleCamera::CSimpleCamera(float nearZ, float farZ, float fov) {
    this->m_scene = nullptr;
    this->m_facing.a0 = 1.0f;
    this->m_facing.a1 = 0.0f;
    this->m_facing.a2 = 0.0f;
    this->m_facing.b0 = 0.0f;
    this->m_facing.b1 = 1.0f;
    this->m_facing.b2 = 0.0f;
    this->m_facing.c0 = 0.0f;
    this->m_facing.c1 = 0.0f;
    this->m_facing.c2 = 1.0f;
    this->m_nearZ = nearZ;
    this->m_farZ = farZ;
    this->m_fov = fov;
    this->m_aspect = 1.0f;
    this->m_facing.FromEulerAnglesZYX(0.0f, 0.0f, 0.0f);
}

// INLINED
CSimpleCamera::~CSimpleCamera() {
    if (this->m_scene) {
        this->m_scene->Release();
    }
}

// VERIFY: 3b9b73df auto
// OFFSET: 0x00607C00
CM2Scene* CSimpleCamera::GetScene() {
    if (!this->m_scene) {
        this->m_scene = M2CreateScene();
    }

    return this->m_scene;
}

// VERIFY: a99af1d8 auto
// OFFSET: 0x00607BD0
void CSimpleCamera::SetFacing(float yaw, float pitch, float roll) {
    this->m_facing.FromEulerAnglesZYX(yaw, pitch, roll);
}

// VERIFY: bc54e001 auto
// OFFSET: 0x00607CB0
void CSimpleCamera::SetGxProjectionAndView(const CRect& rect) {
    C44Matrix projection;
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
    GxuXformCreateProjection_Exact(this->FOV() * 0.6f, this->m_aspect, this->m_nearZ, this->m_farZ, projection);
    g_theGxDevicePtr->XformSetProjection(projection);

    C44Matrix view;
    C3Vector eye;
    GxuXformCreateLookAtSgCompat(eye, this->Forward(), this->Up(), view);
    g_theGxDevicePtr->XformSetView(view);
}

// VERIFY: 62c62fe7 auto
// OFFSET: 0x00607C90
void CSimpleCamera::SetScreenAspect(const CRect& rect) {
    this->m_aspect = (rect.maxX - rect.minX) / (rect.maxY - rect.minY);
}
