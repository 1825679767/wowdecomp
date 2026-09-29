#include "VehiclePassenger_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/Object.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// STATIC: 0x00CA135C
uint32_t* CVehiclePassenger_C::s_passengerPool;

// STATIC: 0x00CA1360
TSGrowableArray<CVehiclePassenger_C*>* CVehiclePassenger_C::s_pendingPassengers;

// STATIC: 0x00CA1364
uint32_t CVehiclePassenger_C::s_pendingCount;

// STATIC: 0x00CA1368
CVehiclePassenger_C* CVehiclePassenger_C::s_headPassenger;

// STATIC: 0x00CA136C
CVehiclePassenger_C* CVehiclePassenger_C::s_tailPassenger;

// STATIC: 0x00CA1358
TSGrowableArray<VehiclePassengerRescueTransition>* CVehiclePassenger_C::s_rescueTransitions;

// VERIFY: c1cea68b auto
// OFFSET: 0x00747900
void CVehiclePassenger_C::ClearFlag8() {
    this->m_flags &= ~0x8;
}

// VERIFY: c6078f8d auto
// OFFSET: 0x00747910
void CVehiclePassenger_C::DeleteAllDeferredVehicleTransitionAnimSplines() {
    if (this->m_deferredTransitionSpline) {
        delete this->m_deferredTransitionSpline;
        this->m_deferredTransitionSpline = nullptr;
    }
}

// VERIFY: 31cbea9a manual
// OFFSET: 0x00747930
int32_t CVehiclePassenger_C::IsWaitingForVehicleTransitionAnimGrabEvent(uint32_t eventId) const {
    uint32_t flags = this->m_flags;
    uint8_t index = static_cast<uint8_t>(eventId >> 24);
    return (flags & 0x200) && !(flags & 0x80) && (flags & 0x100) && (index == '0' || this->m_transitionAnimSeat == index - '0' - 1);
}

// VERIFY: 3edd387b auto
// OFFSET: 0x00747980
void CVehiclePassenger_C::OnVehicleRideLoopAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers) {
    done = 1;
}

// VERIFY: 0697d6f6 auto
// OFFSET: 0x00747A30
void CVehiclePassenger_C::UpdateSmoothFacing() {
    this->m_targetFacing = this->m_unit->GetFacing();

    while (this->m_targetFacing + 3.1415927f < this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing + 6.2831855f;
    }

    while (this->m_targetFacing - 3.1415927f > this->m_smoothFacing) {
        this->m_targetFacing = this->m_targetFacing - 6.2831855f;
    }

    this->m_smoothFacing = (1.0f - this->m_facingBlend) * this->m_startFacing + this->m_targetFacing * this->m_facingBlend;
}

// VERIFY: 2daee16a auto
// OFFSET: 0x00747AE0
void CVehiclePassenger_C::PostRenderUpdateAll() {
    for (CVehiclePassenger_C* passenger = s_headPassenger; passenger; passenger = passenger->m_next) {
        passenger->m_flags &= ~0x1000;
    }
}

// VERIFY: 469c80f9 auto
// OFFSET: 0x00747B00
void CVehiclePassenger_C::LockWorldFacingNextFrame(float facing) {
    this->m_flags |= 0x1000;
    this->m_lockedWorldFacing = facing;
}

// VERIFY: 07557628 manual
// OFFSET: 0x00747FF0
CGUnit_C* CVehiclePassenger_C::GetVehicleUnit() const {
    if (!this->m_transitionVehicleGuid) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return unit;
}

// VERIFY: aa771f7c manual
// OFFSET: 0x00748170
void CVehiclePassenger_C::StartDeferringVehicleTransitionAnimSpline(CDataStore* msg) {
    CDataStore* spline = new (SMemAlloc(sizeof(CDataStore), __FILE__, __LINE__, 0)) CDataStore();
    this->m_deferredTransitionSpline = spline;

    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    spline->PutData(data, bytes);
    this->m_deferredTransitionSpline->Finalize();
}

// VERIFY: a002108c manual
// OFFSET: 0x007481E0
int32_t CVehiclePassenger_C::IsWaitingForVehicleTransitionAnimThrowEvent(uint32_t eventId) const {
    uint32_t flags = this->m_flags;
    uint8_t index = static_cast<uint8_t>(eventId >> 24);
    return (flags & 0x200) && !(flags & 0x80) && !(flags & 0x100) && (index == '0' || static_cast<uint8_t>(this->m_unit->m_movementData.m_transportSeat) == index - '0' - 1);
}

// VERIFY: 1f5a0a4e auto
// OFFSET: 0x007484C0
float CVehiclePassenger_C::GetLockedWorldFacing() const {
    if (this->m_flags & 0x1000) {
        return this->m_lockedWorldFacing;
    }

    return this->m_unit->GetFacing();
}

// VERIFY: 02fee933 auto
// OFFSET: 0x007488F0
CVehiclePassenger_C::CVehiclePassenger_C() {
}

// VERIFY: 25b3b5b5 auto
// OFFSET: 0x00748950
void CVehiclePassenger_C::Free() {
    if (this == s_headPassenger) {
        s_headPassenger = this->m_next;

        if (s_headPassenger) {
            s_headPassenger->m_prev = nullptr;
        }
    } else if (this == s_tailPassenger) {
        s_tailPassenger = this->m_prev;

        if (s_tailPassenger) {
            s_tailPassenger->m_next = nullptr;
        }
    } else {
        this->m_prev->m_next = this->m_next;
        this->m_next->m_prev = this->m_prev;
    }

    if (this->m_deferredTransitionSpline) {
        delete this->m_deferredTransitionSpline;
    }

    ObjectFree(*s_passengerPool, this->m_memHandle);
}

// VERIFY: 664149dd manual
// OFFSET: 0x00749060
CVehicle_C* CVehiclePassenger_C::GetVehicle() const {
    CGUnit_C* unit;

    if (!this->m_transitionVehicleGuid || !(unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (unit) {
        return unit->m_vehicle;
    }

    return nullptr;
}

// VERIFY: bcb00fbb manual
// OFFSET: 0x00749C20
void CVehiclePassenger_C::AddPendingRescueTransition(WOWGUID passengerGuid, WOWGUID vehicleGuid, uint8_t seat, int32_t instant) {
    VehiclePassengerRescueTransition* transition = s_rescueTransitions->New();
    transition->m_passengerGuid = passengerGuid;
    transition->m_vehicleGuid = vehicleGuid;
    transition->m_seat = seat;
    transition->m_instant = instant != 0;
}

// VERIFY: c31d4cfc manual
// OFFSET: 0x00749D50
void CVehiclePassenger_C::OnVehicleTransitionAnimEnd(CGUnit_C* unit, int32_t& done, TSGrowableArray<CVehiclePassenger_C*>& passengers) {
    done = 0;
    CVehiclePassenger_C* passenger = unit->m_vehiclePassenger;

    if (passenger && static_cast<int32_t>(passenger->m_flags) >= 0) {
        passengers.New(passenger);
    }
}

// VERIFY: 8979bf0a manual
// OFFSET: 0x0074A070
void CVehiclePassenger_C::InitSystem() {
    delete s_passengerPool;
    delete s_pendingPassengers;

    uint32_t* pool = static_cast<uint32_t*>(SMemAlloc(sizeof(uint32_t), __FILE__, __LINE__, 0));

    if (pool) {
        *pool = ObjectAllocAddHeap(sizeof(CVehiclePassenger_C), 16, "Vehicle Passenger", true);
        s_passengerPool = pool;
    } else {
        s_passengerPool = nullptr;
    }

    void* pending = SMemAlloc(sizeof(TSGrowableArray<CVehiclePassenger_C*>), __FILE__, __LINE__, 0);
    s_pendingPassengers = pending ? new (pending) TSGrowableArray<CVehiclePassenger_C*>() : nullptr;
    s_pendingCount = 0;

    void* rescue = SMemAlloc(sizeof(TSGrowableArray<VehiclePassengerRescueTransition>), __FILE__, __LINE__, 0);
    s_headPassenger = nullptr;
    s_tailPassenger = nullptr;
    s_rescueTransitions = rescue ? new (rescue) TSGrowableArray<VehiclePassengerRescueTransition>() : nullptr;
}

// VERIFY: 93875c7e manual
// OFFSET: 0x0074A160
void CVehiclePassenger_C::UninitSystem() {
    if (s_passengerPool) {
        delete s_passengerPool;
        s_passengerPool = nullptr;
    }

    if (s_pendingPassengers) {
        delete s_pendingPassengers;
        s_pendingPassengers = nullptr;
    }

    s_pendingCount = 0;

    if (s_rescueTransitions) {
        delete s_rescueTransitions;
        s_rescueTransitions = nullptr;
    }

    s_tailPassenger = nullptr;
    s_headPassenger = nullptr;
}

// VERIFY: 7b346c04 manual
// OFFSET: 0x0074B160
void CVehiclePassenger_C::StoreRootVehicle(WOWGUID guid) {
    this->m_rootVehicleGuid = guid;

    if (!guid) {
        guid = this->m_unit->m_obj->m_guid;
    }

    CVehicle_C* vehicle = this->m_unit->m_vehicle;

    if (!vehicle) {
        return;
    }

    for (CPassenger* passenger = vehicle->m_passengers.Head(); passenger; passenger = vehicle->m_passengers.Next(passenger)) {
        CVehiclePassenger_C* vehiclePassenger = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_UNIT, __FILE__, __LINE__))->m_vehiclePassenger;

        if (vehiclePassenger) {
            vehiclePassenger->StoreRootVehicle(guid);
        }
    }
}

// VERIFY: 8a84e9f8 auto
// OFFSET: 0x007479E0
void CVehiclePassenger_C::StopBoneAnimIfSameBone(WOWGUID guid, const VehicleSeatRec* seat) {
    if (guid != this->m_boneAnimGuid) {
        return;
    }

    int32_t enterBone = seat->m_vehicleEnterAnimBone;

    if (enterBone >= 35) {
        enterBone = 26;
    }

    int32_t exitBone = seat->m_vehicleExitAnimBone;

    if (exitBone >= 35) {
        exitBone = 26;
    }

    if (enterBone == exitBone) {
        this->m_boneAnimGuid = WOWGUID();
    }
}

// VERIFY: 342659bc manual
// OFFSET: 0x00748070
void CVehiclePassenger_C::StartWaitingForVehicleTransitionAnim(WOWGUID guid, uint8_t seat, const VehicleSeatRec* seatRec, int32_t entering, CVehicle_C* vehicle) {
    this->m_flags |= 0x200;
    uint32_t flags = this->m_flags;
    this->m_transitionAnimVehicleGuid = vehicle->m_unit->m_obj->m_guid;
    this->m_transitionAnimGuid = guid;
    this->m_transitionAnimSeat = seat;

    if (seatRec->m_flags & (entering ? 0x8000000 : 0x10000000)) {
        float duration = entering ? seatRec->m_field33512213037 : seatRec->m_field33512213038;
        uint32_t timeout = static_cast<uint32_t>(duration * 1000.0f);

        if (timeout > 3000) {
            timeout = 3000;
        }

        this->m_flags = flags | 0x80;
        this->m_transitionAnimTimeout = timeout + FrameTime::s_curTimeMs;
    } else {
        this->m_flags = flags & ~0x80;

        if (entering) {
            this->m_flags |= 0x100;
        } else {
            this->m_flags &= ~0x100;
        }

        this->m_transitionAnimTimeout = FrameTime::s_curTimeMs + 3000;
    }
}

// VERIFY: 06e886f0 manual
// OFFSET: 0x00748400
void CVehiclePassenger_C::ComputeSeatOffset(const VehicleSeatRec* seat, CM2Model* model) {
    if (!seat || !model || !model->IsLoaded(0, 0)) {
        return;
    }

    uint32_t attachment = seat->m_passengerAttachmentID;

    if (model->HasAttachment(attachment)) {
        C3Vector pivot = model->GetAttachmentPivot(attachment);
        float x = -pivot.x;
        float y = -pivot.y;
        float z = -pivot.z;
        this->m_flags |= 0x40;
        this->m_flags |= 0x20;
        this->m_seatOffset.x = x;
        this->m_seatOffset.y = y;
        this->m_seatOffset.z = z;
    } else {
        this->m_seatOffset.x = 0.0f;
        this->m_seatOffset.y = 0.0f;
        this->m_seatOffset.z = 0.0f;
        this->m_flags &= ~0x40;
        this->m_flags |= 0x20;
    }
}

// VERIFY: ac9be9b7 auto
// OFFSET: 0x007485B0
int32_t CVehiclePassenger_C::ChooseSeatExitAnim(uint32_t flags, uint32_t* anim) const {
    uint32_t state = this->m_state;

    if (state != 3 && this->m_unit->m_unit->m_unitFieldHealth > 0) {
        return 0;
    }

    const VehicleSeatRec* seat = this->m_seatRec;

    if (!seat || state != 3 || !(seat->m_flags & 0x4)) {
        return 0;
    }

    int32_t exitAnim;

    if ((this->m_flags & 0x4) || seat->m_rideUpperAnimStart == -1) {
        exitAnim = seat->m_rideUpperAnimLoop;

        if (exitAnim == -1) {
            return 0;
        }
    } else {
        exitAnim = seat->m_rideUpperAnimStart;
    }

    if (exitAnim != 506) {
        *anim = exitAnim;
        return 1;
    }

    return 0;
}

// VERIFY: f673db8c auto
// OFFSET: 0x00747B20
int32_t CVehiclePassenger_C::GetLowerAnim(const VehicleSeatRec* seat) const {
    if (!seat) {
        return 506;
    }

    int32_t anim;

    switch (this->m_state) {
        case 1:
        case 2: {
            if (!(seat->m_flags & 0x1)) {
                return 506;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_enterAnimStart) == -1) {
                anim = seat->m_enterAnimLoop;

                if (anim == -1) {
                    return 506;
                }
            }

            break;
        }

        case 3: {
            if (!(seat->m_flags & 0x2)) {
                return 506;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_rideAnimStart) == -1) {
                anim = seat->m_rideAnimLoop;

                if (anim == -1) {
                    return 506;
                }
            }

            break;
        }

        case 4:
        case 5: {
            if (!this->m_unit->ShouldUseVehicleSeatExitAnimation(seat)) {
                return 506;
            }

            if ((this->m_flags & 0x2) || (anim = seat->m_exitAnimStart) == -1) {
                anim = seat->m_exitAnimLoop;

                if (anim == -1) {
                    return 506;
                }
            }

            break;
        }

        default: {
            return 506;
        }
    }

    return anim;
}

// VERIFY: a4d47b4a manual
// OFFSET: 0x00748040
CGUnit_C* CVehiclePassenger_C::GetStoredRootVehicleUnit() const {
    if (this->m_state) {
        return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_rootVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    return this->m_unit->FindRootVehicle(nullptr);
}

// VERIFY: 4f73af22 manual
// OFFSET: 0x00748620
void CVehiclePassenger_C::StartMirroringVehicleAnims() {
    if (this->m_transitionSeat >= 8) {
        return;
    }

    CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_transitionVehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicleUnit || !vehicleUnit->m_vehicle) {
        return;
    }

    vehicleUnit->m_vehicle->OnPassengerStartMirroringAnims(this->m_transitionSeat);
    this->m_flags |= 0x800;

    if (this->m_unit->m_unit->m_unitFieldHealth <= 0) {
        return;
    }

    CM2Model* vehicleModel = vehicleUnit->GetObjectModel();
    CM2Model* model = this->m_unit->GetObjectModel();

    if (!vehicleModel || !model || !vehicleModel->IsLoaded(0, 0) || !model->IsLoaded(0, 0)) {
        return;
    }

    M2BoneSequenceInfo info;
    vehicleModel->GetBoneSequenceInfo(0xFFFFFFFF, info);

    if (info.m_sequenceId >= 506) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1);
    } else {
        model->SetBoneSequence(0xFFFFFFFF, info.m_sequenceId, info.m_variationIndex, info.m_time, info.m_speed, 1, 1);
    }

    for (uint32_t bone = 0; bone < 35; bone++) {
        int32_t vehicleHasBone = vehicleModel->HasBone(bone);
        int32_t hasBone = model->HasBone(bone);

        if (vehicleHasBone) {
            if (!hasBone) {
                continue;
            }

            vehicleModel->GetBoneSequenceInfo(bone, info);

            if (info.m_sequenceId < 506) {
                model->SetBoneSequence(bone, info.m_sequenceId, info.m_variationIndex, info.m_time, info.m_speed, 1, 1);
                continue;
            }
        } else if (!hasBone) {
            continue;
        }

        if (model->GetBoneSequenceId(bone) != 0xFFFFFFFF) {
            model->UnsetBoneSequence(bone, 0, 1);
        }
    }

    if (vehicleModel->m_lastAnimTime) {
        uint32_t time = model->m_scene->m_time;

        if (!time) {
            time = 1;
        }

        model->m_lastAnimTime = time;
    } else {
        model->m_lastAnimTime = 0;
    }
}

// VERIFY: 5f117fc1 manual
// OFFSET: 0x00749790
CVehiclePassenger_C* CVehiclePassenger_C::Alloc(CGUnit_C* unit) {
    uint32_t memHandle;
    void* memory = nullptr;
    CVehiclePassenger_C* passenger = reinterpret_cast<CVehiclePassenger_C*>(ObjectAlloc(*s_passengerPool, &memHandle, &memory, false));

    if (passenger) {
        passenger = static_cast<CVehiclePassenger_C*>(memory);

        if (passenger) {
            new (passenger) CVehiclePassenger_C();
        }

        passenger->m_memHandle = memHandle;
    }

    passenger->m_facingBlend = 0.0f;
    passenger->m_unit = unit;
    passenger->m_flags = 0;
    passenger->m_state = 0;
    passenger->unk_0018[0] = 0;
    passenger->m_vehicleGuid = WOWGUID();
    passenger->m_transitionVehicleGuid = WOWGUID();
    passenger->unk_0034[0] = 0;
    passenger->unk_0034[1] = 0;
    passenger->m_startRelativeGuid = WOWGUID();
    passenger->unk_004C[0] = 0;
    passenger->unk_004C[1] = 0;
    passenger->m_seatRec = nullptr;
    passenger->m_rootVehicleGuid = WOWGUID();
    passenger->m_deferredTransitionSpline = nullptr;
    passenger->m_transitionAnimTimeout = 0;
    passenger->m_transitionAnimVehicleGuid = WOWGUID();
    passenger->m_boneAnimGuid = WOWGUID();
    passenger->m_transitionAnimGuid = WOWGUID();
    passenger->m_seat = 0xFF;
    passenger->m_transitionSeat = 0xFF;
    passenger->m_transitionAnimSeat = 0xFF;
    passenger->unk_0084 = C3Vector();
    passenger->unk_0090 = C3Vector();
    passenger->m_startPosition = C3Vector();
    passenger->m_startOffset = C3Vector();
    passenger->m_startFacing = 0.0f;
    passenger->unk_00B8 = 0;
    passenger->m_smoothFacing = 0.0f;
    passenger->m_targetFacing = 0.0f;
    passenger->unk_00C4[0] = 0;
    passenger->unk_00C4[1] = 0;
    passenger->m_seatOffset = C3Vector();
    passenger->m_lockedWorldFacing = 0.0f;
    passenger->m_prev = nullptr;

    CVehiclePassenger_C* head = s_headPassenger;
    s_headPassenger = passenger;

    if (!head) {
        passenger->m_next = nullptr;
        s_tailPassenger = passenger;
    } else {
        passenger->m_next = head;
        head->m_prev = passenger;
    }

    return passenger;
}

// VERIFY: 09b1b72f manual
// OFFSET: 0x00749A30
void CVehiclePassenger_C::Uninit() {
    if (this->m_flags & 0x800) {
        this->m_flags &= ~0x800;

        if (this->m_seat < 8) {
            CGUnit_C* vehicleUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (vehicleUnit && vehicleUnit->m_vehicle) {
                vehicleUnit->m_vehicle->OnPassengerStopMirroringAnims(this->m_seat);
            }
        }
    }

    CVehicle_C* vehicle = this->GetVehicle();

    if (vehicle) {
        vehicle->UpdateFreeSeatMask();
    }

    this->Free();
}

// VERIFY: a5679fee manual
// OFFSET: 0x00747E90
C3Vector CVehiclePassenger_C::ComputeStartWorldPosition(const C3Vector& offset, const C3Vector& fallback) const {
    CGObject_C* relative = ClntObjMgrObjectPtr(this->m_startRelativeGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (relative) {
        if (relative->m_obj->m_type & TYPEMASK_UNIT) {
            C3Vector position = static_cast<CGUnit_C*>(relative)->BuildSmoothPosition();
            return C3Vector(offset.x + position.x, position.y + offset.y, position.z + offset.z);
        }

        return offset * relative->GetMatrix();
    }

    return fallback;
}

// VERIFY: b021a3dc auto
// OFFSET: 0x007484E0
void CVehiclePassenger_C::OnAnimEnd(uint32_t anim) {
    const VehicleSeatRec* seat = this->m_seatRec;

    if (this->GetLowerAnim(seat) != 506 && seat && this->m_state == 3 && (seat->m_flags & 0x4)) {
        int32_t upper;

        if ((this->m_flags & 0x4) || (upper = seat->m_rideUpperAnimStart) == -1) {
            upper = seat->m_rideUpperAnimLoop;

            if (upper == -1) {
                this->m_flags |= 0x6;
                return;
            }
        }

        if (upper != 506) {
            if (anim == 0xFFFFFFFF || anim == 26) {
                this->m_flags |= 0x2;
            } else {
                this->m_flags |= 0x4;
            }

            return;
        }
    }

    this->m_flags |= 0x6;
}

// VERIFY: 9156af0b auto
// OFFSET: 0x00748560
int32_t CVehiclePassenger_C::PickHighPriorityAnimation(int32_t a2, uint32_t* anim) const {
    if ((this->m_state == 0 || this->m_state == 3) && this->m_unit->m_unit->m_unitFieldHealth > 0) {
        return 0;
    }

    if (!this->m_seatRec) {
        return 0;
    }

    int32_t lower = this->GetLowerAnim(this->m_seatRec);

    if (lower == 506) {
        return 0;
    }

    *anim = lower;
    return 1;
}

// VERIFY: e55483fa auto
// OFFSET: 0x007482A0
C3Vector CVehiclePassenger_C::ComputeInterpolatedUnitPosition() const {
    switch (this->m_state) {
        case 1:
        case 4:
            if (this->m_flags & 0x1) {
                return this->ComputeStartWorldPosition(this->m_startOffset, this->m_startPosition);
            }

            return this->m_startPosition;
        case 2:
        case 5:
            if (this->m_flags & 0x1) {
                float blend = this->m_facingBlend;
                C3Vector smooth = this->m_unit->BuildSmoothPosition();
                C3Vector end(blend * smooth.x, smooth.y * blend, blend * smooth.z);
                float inverse = 1.0f - this->m_facingBlend;
                C3Vector start = this->ComputeStartWorldPosition(this->m_startOffset, this->m_startPosition);
                return C3Vector(start.x * inverse + end.x, start.y * inverse + end.y, start.z * inverse + end.z);
            }

            return this->m_startPosition;
        case 3:
            return this->m_unit->BuildSmoothPosition();
        default:
            return this->m_unit->GetPosition();
    }
}

// VERIFY: 1e2a1e8c auto
// OFFSET: 0x007487E0
int32_t CVehiclePassenger_C::OverridesModelAnimation() const {
    return (this->m_flags & 0x800) && this->m_unit->m_unit->m_unitFieldHealth > 0;
}

// VERIFY: 1c7d54aa auto
// OFFSET: 0x007478F0
void CVehiclePassenger_C::StartAddingPendingRescueTransitions() {
    CVehiclePassenger_C::s_pendingCount = 1;
}
