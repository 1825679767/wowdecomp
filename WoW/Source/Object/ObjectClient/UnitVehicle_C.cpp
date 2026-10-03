#include "UnitVehicle_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/WorldClient/FrameTime.h"

// VERIFY: bc21d620 manual
// OFFSET: 0x0074C650
CGUnit_C* CGUnit_C::FindRootVehicle(CGUnit_C* stop) {
    CGUnit_C* unit = this;

    if (this == stop && this->m_vehicle && this->m_vehicle->m_vehicleRec) {
        return this;
    }

    WOWGUID guid = { this->GetTransportGUID() };
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return nullptr;
        }

        while (unit != stop) {
            guid.m_guid = unit->GetTransportGUID();
            high = static_cast<uint32_t>(guid.m_guid >> 32);

            if ((high & 0xF0F00000) != 0xF0500000 && ((high & 0xF0000000) || !((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
                break;
            }

            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                return nullptr;
            }
        }

        return unit;
    }

    if (unit->m_vehicle && unit->m_vehicle->m_vehicleRec) {
        return unit;
    }

    return nullptr;
}

// VERIFY: 26588931 manual
// OFFSET: 0x0074CC40
CGUnit_C* CGUnit_C::FindRootVehicleOfPassengerChain(CGUnit_C* stop) {
    CGUnit_C* unit = this;
    WOWGUID guid = this->m_vehiclePassenger ? this->m_vehiclePassenger->m_transitionVehicleGuid : WOWGUID();

    if (!guid) {
        if (!this->m_vehicle || !this->m_vehicle->m_vehicleRec) {
            return nullptr;
        }

        return unit;
    }

    unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return nullptr;
    }

    while (unit != stop) {
        guid = unit->m_vehiclePassenger ? unit->m_vehiclePassenger->m_transitionVehicleGuid : WOWGUID();

        if (!guid) {
            break;
        }

        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return nullptr;
        }
    }

    return unit;
}

// VERIFY: 9db634f7 manual
// OFFSET: 0x0074B900
bool CGUnit_C::IsVehiclePreventingTurning() const {
    uint32_t high = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid >> 32);
    uint32_t low = static_cast<uint32_t>(this->m_movementData.m_transportGuid.m_guid);

    if ((high & 0xF0F00000) != 0xF0500000 && ((high & 0xF0000000) || !((high & 0xF07FFFFF) | low))) {
        return false;
    }

    CGUnit_C* transport = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ static_cast<uint64_t>(high) << 32 | low }, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!transport || !transport->m_vehicle) {
        return false;
    }

    VehicleSeatRec* seat = transport->m_vehicle->GetVehicleSeatRec(static_cast<uint8_t>(this->m_movementData.m_transportSeat));
    return seat && !(seat->m_flags & 0x400);
}

// VERIFY: 08c11781 manual
// OFFSET: 0x0074BA40
bool CGUnit_C::IsAlteredFormTransitionPreventingMovement() const {
    if (!this->IsLocalClientControlled()) {
        return false;
    }

    CVehiclePassenger_C* passenger = this->m_vehiclePassenger;

    if (passenger && passenger->m_state && passenger->m_state != 3) {
        return true;
    }

    if (passenger && (passenger->m_flags & 0x200)) {
        return true;
    }

    CGUnit_C* charmer = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit->m_unitFieldCharmedby, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (charmer) {
        CVehiclePassenger_C* charmerPassenger = charmer->m_vehiclePassenger;

        if (!charmerPassenger || ((!charmerPassenger->m_state || charmerPassenger->m_state == 3) && !(charmerPassenger->m_flags & 0x200))) {
            return charmer->IsInVehicle() && (charmerPassenger->m_flags & 0x8);
        }

        return true;
    }

    if (!(this->m_unit->m_unitFieldFlags & 0x1000000) && this->m_obj->m_guid != ClntObjMgrGetActivePlayer()) {
        return true;
    }

    return false;
}

// VERIFY: 97008da9 manual
// OFFSET: 0x0074C550
void CGUnit_C::ConvertVehicleCameraRelativeFacingToTransportRelativeFacing(float& facing) {
    if (this->m_vehicleCamera) {
        CGObject_C* relativeTo = ClntObjMgrObjectPtr(WOWGUID{ this->m_vehicleCamera->GetRelativeTo() }, TYPEMASK_OBJECT, __FILE__, __LINE__);
        CVehicleCamera_C::ConvertFromRawToWorld(facing, relativeTo);
        facing = this->m_movement->GetRawFacing(facing);
    }
}

// VERIFY: a5b201a2 manual
// OFFSET: 0x0074C5A0
void CGUnit_C::UpdateVehicleUISliders(CGUnit_C* unit, const VehicleRec* vehicleRec) {
    int32_t enable = unit && !unit->IsAlteredFormTransitionPreventingMovement();

    if (enable && vehicleRec && (vehicleRec->m_flags & 0x400)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, "%d", 1);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_SHOW, nullptr);
    }

    if (enable && vehicleRec && (vehicleRec->m_flags & 0x800)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, "%d", 1);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_POWER_SHOW, nullptr);
    }

    if (unit) {
        CGInputControl::GetActive();
        CGInputControl::SendUIVehicleAngleUpdate(unit->GetPitch());
    }
}

// VERIFY: 021f7c45 manual
// OFFSET: 0x0074BCB0
int32_t CGUnit_C::VehiclePassengerShouldWaitForVehicleTransitionAnim(uint64_t transport, uint8_t seat) const {
    if (transport == this->GetTransportGUID() && seat == this->m_movementData.m_transportSeat) {
        return 0;
    }

    uint32_t high = static_cast<uint32_t>(transport >> 32);
    bool entering = (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport)));
    WOWGUID guid;

    if (entering) {
        guid.m_guid = transport;
    } else {
        if (!this->HasVehicleTransport()) {
            return 0;
        }

        guid.m_guid = this->GetTransportGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CVehicle_C* vehicle = unit->m_vehicle;

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seatRec = vehicle->GetVehicleSeatRec(entering ? seat : this->m_movementData.m_transportSeat);

    if (!seatRec) {
        return 0;
    }

    CVehicle_C* animVehicle = vehicle->FindVehicleToPlayBoneAnim();

    if (entering) {
        if (!(seatRec->m_flags & 0x400000)) {
            return 0;
        }

        uint32_t anim = seatRec->m_vehicleEnterAnim;

        if (anim > 0x1F9) {
            return 0;
        }

        return animVehicle->CanPlayBoneAnim(anim) ? 1 : 0;
    }

    if (!(seatRec->m_flags & ((this->m_movementData.m_flags2 & 0x40) ? 0x40000 : 0x80000))) {
        return 0;
    }

    uint32_t anim = seatRec->m_vehicleExitAnim;
    return anim <= 0x1F9 && animVehicle->CanPlayBoneAnim(anim);
}

// VERIFY: 0d563d24 manual
// OFFSET: 0x0074BE10
int32_t CGUnit_C::VehiclePassengerStartWaitingForVehicleTransitionAnim(uint64_t transport, uint8_t seat) {
    uint32_t high = static_cast<uint32_t>(transport >> 32);
    int32_t entering = (high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport)));
    WOWGUID guid;

    if (entering) {
        guid.m_guid = transport;
    } else {
        if (!this->HasVehicleTransport()) {
            return 0;
        }

        guid.m_guid = this->GetTransportGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CVehicle_C* vehicle = unit->m_vehicle;

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seatRec = vehicle->GetVehicleSeatRec(entering ? seat : this->m_movementData.m_transportSeat);

    if (!seatRec) {
        return 0;
    }

    if (this->m_vehiclePassenger) {
        this->m_vehiclePassenger->StopWaitingForVehicleTransitionAnim();
    }

    CVehicle_C* animVehicle = vehicle->FindVehicleToPlayBoneAnim();

    if (entering) {
        if (!(seatRec->m_flags & 0x400000) || static_cast<uint32_t>(seatRec->m_vehicleEnterAnim) > 0x1F9 || !animVehicle->PlayBoneAnim(seatRec->m_field33512213034, seatRec->m_vehicleEnterAnim)) {
            return 0;
        }

        animVehicle->InstallBoneAnimListener(this->m_obj->m_guid, seatRec->m_field33512213034, &CVehiclePassenger_C::OnVehicleTransitionAnimEnd);

        if (seatRec->m_flagsB & 0x800000) {
            animVehicle->SetBoneAnimForceFacing(this->m_obj->m_guid, seatRec->m_field33512213034);
        }
    } else {
        if (!(seatRec->m_flags & ((this->m_movementData.m_flags2 & 0x40) ? 0x40000 : 0x80000)) || static_cast<uint32_t>(seatRec->m_vehicleExitAnim) > 0x1F9 || !animVehicle->PlayBoneAnim(seatRec->m_vehicleExitAnimBone, seatRec->m_vehicleExitAnim)) {
            return 0;
        }

        if (this->m_vehiclePassenger) {
            this->m_vehiclePassenger->StopBoneAnimIfSameBone(animVehicle->m_unit->m_obj->m_guid, seatRec);
        }

        animVehicle->InstallBoneAnimListener(this->m_obj->m_guid, seatRec->m_vehicleExitAnimBone, &CVehiclePassenger_C::OnVehicleTransitionAnimEnd);
    }

    if (!this->m_vehiclePassenger) {
        this->m_vehiclePassenger = CVehiclePassenger_C::Alloc(this);
    }

    this->m_vehiclePassenger->StartWaitingForVehicleTransitionAnim(WOWGUID{ transport }, seat, seatRec, entering, animVehicle);
    return 1;
}

// VERIFY: 65e43ba1 manual
// OFFSET: 0x0074CD60
void CGUnit_C::DestroyVehicleCamera() {
    if (!this->m_vehicleCamera) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_vehicleCamera->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    CVehicleCamera_C* camera = this->m_vehicleCamera;
    camera->Destroy();
    this->m_vehicleCamera = nullptr;
    CGCamera* activeCamera = CGWorldFrame::GetActiveCamera();

    if (activeCamera && activeCamera->m_vehicleCamera == camera) {
        activeCamera->OnVehicleCameraDestroyed();
    }

    if (unit) {
        unit->OnVehicleCameraPossiblyUnneeded();
    }
}

// VERIFY: 404e4e04 auto
// OFFSET: 0x004F6230
CGUnit_C* CGUnit_C::GetRootVehicleUnit() {
    if (this->m_vehiclePassenger) {
        return this->m_vehiclePassenger->GetStoredRootVehicleUnit();
    }

    return this->FindRootVehicle(nullptr);
}

// VERIFY: 218b4093 auto
// OFFSET: 0x0074B9C0
void CGUnit_C::PlayVehicleExitEndAnimation(const VehicleSeatRec* seat, uint32_t anim) {
    if (!seat || !(seat->m_flags & 0x200000) || anim == 0xFFFFFFFF) {
        uint32_t flags = this->m_movement->m_flags;

        if (flags & 0x1000) {
            this->PlayBaseAnimation(ANIM_FALL, 0);
            this->m_animationState |= 0x1000000;
            return;
        }

        if (flags & 0x200000) {
            this->PlayBaseAnimation(ANIM_SWIM_IDLE, 0);
            return;
        }

        if ((flags & 0x2000000) || anim == 0xFFFFFFFF) {
            return;
        }
    }

    this->PlayBaseAnimation(anim, 1);
}

// VERIFY: d8cede4e auto
// OFFSET: 0x0074C4E0
void CGUnit_C::OnVehicleCameraPossiblyUnneeded() {
    CVehicleCamera_C* camera = this->m_vehicleCamera;

    if (!camera || static_cast<int8_t>(camera->m_flags) < 0) {
        return;
    }

    CGCamera* active = CGWorldFrame::GetActiveCamera();

    if (active) {
        CVehicleCamera_C* current = active->m_vehicleCamera;

        while (current) {
            if (current == this->m_vehicleCamera) {
                return;
            }

            if (!current->m_nextCameraGuid) {
                break;
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(current->m_nextCameraGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                break;
            }

            current = unit->m_vehicleCamera;
        }
    }

    this->DestroyVehicleCamera();
}

// VERIFY: 6c23ff21 auto
// OFFSET: 0x0074CF30
void CGUnit_C::VehiclePassengerOnUpdateTransport(WOWGUID transport, uint8_t seat, int32_t a3) {
    if (!(this->m_clientFlags & 0x80000) && !a3) {
        return;
    }

    CGUnit_C* root = this->m_vehiclePassenger ? this->m_vehiclePassenger->GetStoredRootVehicleUnit() : this->FindRootVehicle(nullptr);

    if (root) {
        root->SetMaxAlpha(1.0f);
    }

    WOWGUID guid = transport;
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);
    CGUnit_C* vehicle;

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid)))) {
        vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!vehicle) {
            guid = WOWGUID{ 0 };
            seat = 0xFF;

            if (!this->m_vehiclePassenger || !this->m_vehiclePassenger->m_transitionVehicleGuid) {
                return;
            }
        }
    } else {
        vehicle = nullptr;
    }

    if (!this->m_vehiclePassenger) {
        if (!guid) {
            return;
        }

        this->m_vehiclePassenger = CVehiclePassenger_C::Alloc(this);
    }

    this->m_vehiclePassenger->OnUpdateTransport(guid, seat, vehicle, a3);

    if (this->m_vehicleCamera) {
        this->m_vehicleCamera->BeginTransition(FrameTime::s_curTimeMs);

        if (!this->m_vehicleCamera->m_state) {
            this->DestroyVehicleCamera();
        }
    } else {
        this->OnVehicleCameraPossiblyNeeded();
    }
}
