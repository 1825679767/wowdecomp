#include "VehicleCamera_C.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/WorldFrame.h"

// STATIC: 0x00CA1658
CVehicleCameraHeap* CVehicleCamera_C::s_cameraPool;

// STATIC: 0x00CA165C
uint32_t CVehicleCamera_C::s_cameraCount;

// VERIFY: 700b924a auto
// OFFSET: 0x00759130
int32_t CVehicleCamera_C::ConvertFromRawToWorld(float& facing, CGObject_C* relativeTo) {
    if (!relativeTo) {
        return 0;
    }

    facing = relativeTo->GetFacing() + facing;
    return 1;
}

// VERIFY: 6565d38c manual
// OFFSET: 0x00759160
void CVehicleCamera_C::InitSystem() {
    delete s_cameraPool;

    CVehicleCameraHeap* pool = static_cast<CVehicleCameraHeap*>(SMemAlloc(sizeof(CVehicleCameraHeap), __FILE__, __LINE__, 0));

    if (pool) {
        pool->m_heapId = ObjectAllocAddHeap(sizeof(CVehicleCamera_C), 4, "Vehicle Camera", true);
        s_cameraPool = pool;
    } else {
        s_cameraPool = nullptr;
    }

    s_cameraCount = 0;
}

// VERIFY: cea6750e manual
// OFFSET: 0x007591D0
void CVehicleCamera_C::UninitSystem() {
    if (s_cameraPool) {
        delete s_cameraPool;
        s_cameraPool = nullptr;
        s_cameraCount = 0;
    }
}

// VERIFY: f89c4583 manual
// OFFSET: 0x007599D0
uint64_t CVehicleCamera_C::GetRelativeTo() const {
    WOWGUID target = this->m_targetGuid;

    if (target == this->m_unit->m_obj->m_guid) {
        return this->m_relativeToGuid.m_guid;
    }

    while (true) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return 0;
        }

        CVehicleCamera_C* camera = unit->m_vehicleCamera;

        if (!camera || !camera->m_state) {
            return unit->GetTransportGUID();
        }

        target = camera->m_targetGuid;

        if (target == camera->m_unit->m_obj->m_guid) {
            return camera->m_relativeToGuid.m_guid;
        }
    }
}

// VERIFY: db488c50 manual
// OFFSET: 0x00759AE0
int32_t CVehicleCamera_C::WouldAddingNextCameraCreateLoop(const CVehicleCamera_C* next) const {
    WOWGUID guid = next->m_nextCameraGuid;

    if (!guid) {
        return 0;
    }

    if (guid == this->m_unit->m_obj->m_guid) {
        return 1;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_vehicleCamera && this->WouldAddingNextCameraCreateLoop(unit->m_vehicleCamera)) {
        return 1;
    }

    return 0;
}

// VERIFY: cfb3b17c manual
// OFFSET: 0x00759B50
int32_t CVehicleCamera_C::ConvertFromRawOrOffsetToWorld(C3Vector& position, CGObject_C* relativeTo) {
    if (!relativeTo) {
        return 0;
    }

    if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
        C3Vector origin = relativeTo->GetPosition();
        position.x = origin.x + position.x;
        position.y = origin.y + position.y;
        position.z = origin.z + position.z;
        return 1;
    }

    position *= relativeTo->GetMatrix();
    return 1;
}

// VERIFY: b4d06088 manual
// OFFSET: 0x00759BD0
int32_t CVehicleCamera_C::ConvertFromWorldToRawOrOffset(C3Vector& position, CGObject_C* relativeTo) {
    if (!relativeTo) {
        return 0;
    }

    if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
        C3Vector origin = relativeTo->GetPosition();
        position.x = position.x - origin.x;
        position.y = position.y - origin.y;
        position.z = position.z - origin.z;
        return 1;
    }

    position *= relativeTo->GetMatrix().AffineInverse();
    return 1;
}

// VERIFY: 6f1e3ea5 manual
// OFFSET: 0x00759C60
int32_t CVehicleCamera_C::ConvertSmoothFacingFromRawToWorld(float& facing, CGObject_C* relativeTo) {
    if (!relativeTo) {
        return 0;
    }

    if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ relativeTo->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);
        float smoothFacing = static_cast<CGUnit_C*>(relativeTo)->m_targetFacing;
        ConvertSmoothFacingFromRawToWorld(smoothFacing, transport);
        facing = smoothFacing + facing;
    } else {
        facing = relativeTo->GetFacing() + facing;
    }

    return 1;
}

// VERIFY: 33f183c1 auto
// OFFSET: 0x00759D60
void CVehicleCamera_C::Destroy() {
    uint32_t memHandle = this->m_memHandle;
    uint32_t heapId = s_cameraPool->m_heapId;
    s_cameraCount--;
    ObjectFree(heapId, memHandle);
}

// VERIFY: 410759cd auto
// OFFSET: 0x0075AA70
CVehicleCamera_C::CVehicleCamera_C() {
}

// VERIFY: 0d3b9410 manual
// OFFSET: 0x007597D0
int32_t CVehicleCamera_C::IsHierarchyChasingFacing() const {
    const CVehicleCamera_C* camera = this;

    while (true) {
        uint32_t state = camera->m_state;
        int32_t transitioning = state == 1 || state == 2 || state == 3;
        const VehicleSeatRec* seat = camera->m_seatRec;

        if (seat) {
            if (!transitioning) {
                int32_t flags = seat->m_flagsB;

                if ((flags & 0x80) && (flags & 0x40000)) {
                    return 1;
                }
            } else {
                if (seat->m_field33512213052 > 0.000099999997f) {
                    return 1;
                }

                if (state == 3) {
                    int32_t flags = seat->m_flagsB;

                    if ((flags & 0x40) && (flags & 0x20000)) {
                        return 1;
                    }
                }
            }
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(camera->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit || !unit->m_vehicleCamera) {
            return 0;
        }

        camera = unit->m_vehicleCamera;
    }
}

// VERIFY: b1bd4cc1 manual
// OFFSET: 0x00759880
int32_t CVehicleCamera_C::UpdateTransitionFlags() {
    uint32_t state = this->m_state;
    int32_t transitioning;
    int32_t smooth;

    if (state != 1 && state != 2 && state != 3) {
        transitioning = 0;
        smooth = 0;
    } else {
        transitioning = 1;
        smooth = this->m_seatRec && (this->m_seatRec->m_flagsB & 0x1);
    }

    if (smooth) {
        this->m_flags |= 0x2;
    } else {
        this->m_flags &= ~0x2;
    }

    int32_t result = 0;

    if (transitioning && this->m_seatRec && (this->m_seatRec->m_flagsB & 0x80001)) {
        result = 1;
    }

    if (result) {
        this->m_flags |= 0x10;
    } else {
        this->m_flags &= ~0x10;
    }

    return result;
}

// VERIFY: a31203e6 manual
// OFFSET: 0x007598F0
float CVehicleCamera_C::GetTransitionProgress(uint32_t time) const {
    uint32_t state = this->m_state;

    if (state == 3) {
        return 1.0f;
    }

    CVehiclePassenger_C* passenger = this->m_unit->m_vehiclePassenger;
    int32_t waiting;
    int32_t mask;

    if (state == 1 || state == 2) {
        waiting = passenger && passenger->m_state == 2;
        mask = 0x40;
    } else {
        waiting = passenger && passenger->m_state == 5;
        mask = 0x80;
    }

    if (waiting && (!this->m_seatRec || !(mask & this->m_seatRec->m_flagsB))) {
        return passenger->m_facingBlend;
    }

    uint32_t duration = this->m_transitionDuration;

    if (!duration) {
        return 1.0f;
    }

    float progress = static_cast<float>(static_cast<int32_t>(time - this->m_transitionStartTime)) / static_cast<float>(static_cast<int32_t>(duration));

    if (progress < 0.0f) {
        return 0.0f;
    }

    if (progress >= 1.0f) {
        return 1.0f;
    }

    return progress;
}

// VERIFY: 75e75aac manual
// OFFSET: 0x00759CE0
int32_t CVehicleCamera_C::ConvertSmoothFacingFromWorldToRaw(float& facing, CGObject_C* relativeTo) {
    if (!relativeTo) {
        return 0;
    }

    if (relativeTo->m_obj->m_type & TYPEMASK_UNIT) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID{ relativeTo->GetTransportGUID() }, TYPEMASK_OBJECT, __FILE__, __LINE__);
        float smoothFacing = static_cast<CGUnit_C*>(relativeTo)->m_targetFacing;
        ConvertSmoothFacingFromRawToWorld(smoothFacing, transport);
        facing = facing - smoothFacing;
    } else {
        facing = facing - relativeTo->GetFacing();
    }

    return 1;
}

// VERIFY: 28de7b3d manual
// OFFSET: 0x0075A720
int32_t CVehicleCamera_C::AttachToHierarchy() {
    CGCamera* activeCamera = CGWorldFrame::GetActiveCamera();

    if (!activeCamera) {
        return 0;
    }

    CVehicleCamera_C* camera = reinterpret_cast<CVehicleCamera_C*>(activeCamera->m_vehicleCamera);

    if (!camera) {
        return 0;
    }

    while (camera->m_nextCameraGuid) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(camera->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return 0;
        }

        camera = unit->m_vehicleCamera;

        if (!camera) {
            return 0;
        }
    }

    CVehiclePassenger_C* passenger = camera->m_unit->m_vehiclePassenger;
    WOWGUID vehicleGuid = passenger ? passenger->m_transitionVehicleGuid : WOWGUID();

    if (vehicleGuid != this->m_unit->m_obj->m_guid || camera->WouldAddingNextCameraCreateLoop(this)) {
        return 0;
    }

    camera->m_nextCameraGuid = this->m_unit->m_obj->m_guid;
    return 1;
}

// VERIFY: 20281449 manual
// OFFSET: 0x00759A60
void CVehicleCamera_C::Unk759A60() {
    CGCamera* activeCamera = CGWorldFrame::GetActiveCamera();

    if (!activeCamera) {
        return;
    }

    for (CVehicleCamera_C* camera = reinterpret_cast<CVehicleCamera_C*>(activeCamera->m_vehicleCamera); camera; ) {
        if (!camera->m_nextCameraGuid) {
            return;
        }

        if (camera->m_nextCameraGuid == this->m_unit->m_obj->m_guid) {
            camera->m_nextCameraGuid = WOWGUID();
            return;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(camera->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return;
        }

        camera = unit->m_vehicleCamera;
    }
}

// VERIFY: 0bd15d9b auto
// OFFSET: 0x0075B150
CVehicleCamera_C* CVehicleCameraHeap::New() {
    uint32_t memHandle;
    void* memory = nullptr;

    if (!ObjectAlloc(this->m_heapId, &memHandle, &memory, false)) {
        return nullptr;
    }

    CVehicleCamera_C* camera = memory ? new (memory) CVehicleCamera_C() : nullptr;
    camera->m_memHandle = memHandle;

    return camera;
}
