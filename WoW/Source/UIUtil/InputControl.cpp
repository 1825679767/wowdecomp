#include <cmath>
#include <cstdint>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/Joystick.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "_Triage/CMoveSpline.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "_Triage/WowMouse.h"

// STATIC: 0x00AD192C
uint32_t s_joystick = 0xFFFFFFFF;

// STATIC: 0x00AD1930
int32_t s_joystickMouseEnabled = 1;

// STATIC: 0x00C24954
CGInputControl* CGInputControl::s_inputControl;

// STATIC: 0x00C24958
static float s_vehicleAimNormPower;

// VERIFY: 13a95e59 auto
// OFFSET: 0x005F95D0
CGInputControl* CGInputControl::GetActive() {
    return CGInputControl::s_inputControl;
}

// VERIFY: b2b6f3af auto
// OFFSET: 0x005F9670
void CGInputControl::UpdateJoystickMouseMode() {
    int32_t enable = s_joystickMouseEnabled && !s_cvCinematicJoystick->m_intValue && (this->m_joystickMouseFlags & 0x2) && (this->m_mouseModeFlags & 0x1);

    if (enable == (this->m_joystickMouseFlags & 0x1)) {
        return;
    }

    if (enable) {
        this->m_joystickMouseFlags |= 0x1;
        OsJoystickSetMouseMode(s_joystick, enable);
    } else {
        this->m_joystickMouseFlags &= ~0x1;
        OsJoystickSetMouseMode(s_joystick, 0);
    }
}

// VERIFY: faf10efa manual
// OFFSET: 0x005F96F0
void CGInputControl::SetMouseCVarHandler(bool enable) {
    if (!this->m_wowMouse) {
        if (enable) {
            this->m_wowMouse = new (SMemAlloc(sizeof(WowMouse), __FILE__, __LINE__, 0)) WowMouse();
        }
    } else if (!enable) {
        delete this->m_wowMouse;
        this->m_wowMouse = nullptr;
    }
}

// VERIFY: 36bc2098 auto
// OFFSET: 0x005FD1D0
CGInputControl::CGInputControl() : m_flags(0), m_lookStartTime(0), m_pendingDefaultAction(PENDING_ACTION_NONE), m_facingOverrideActive(0), m_facingOverride(0.0f), m_vehicleAimValid(0), m_vehicleAim(0.0f), m_mouseModeFlags(3), m_forceCursorOn(0), m_joystickMouseFlags(3), m_joystickLookX(0.0f), m_joystickLookY(0.0f), m_wowMouse(nullptr) {
    this->m_time = OsGetAsyncTimeMs();
}

// VERIFY: 67293d46 manual
// OFFSET: 0x005FD2C0
void InputControlInitialize() {
    CVar::Register("Joystick", "enable joystick control", 0, "0", JoystickCallback, 5, false, nullptr, false);
    s_cvCinematicJoystick = CVar::Register("CinematicJoystick", "enable cinematic joystick control", 0, "0", nullptr, 5, false, nullptr, false);
    CGInputControl::s_inputControl = new (SMemAlloc(sizeof(CGInputControl), __FILE__, __LINE__, 0)) CGInputControl();
    s_cvEnableWowMouse = CVar::Register("enableWowMouse", "Enable Steelseries World of Warcraft Mouse", 1, "0", WowMouseCVarCallback, 5, false, nullptr, false);
}

// VERIFY: f53f4378 auto
// OFFSET: 0x005FA890
void CGInputControl::UpdateMouseMode(int32_t force) {
    uint32_t flags = this->m_mouseModeFlags;
    uint32_t cursorOn;

    if (flags & 0x20) {
        cursorOn = 1;
    } else if (flags & 0x40) {
        cursorOn = 0;
    } else if (s_cvCinematicJoystick->m_intValue || (flags & 0x10) || this->m_forceCursorOn) {
        cursorOn = 1;
    } else {
        cursorOn = flags & 0x6;
    }

    if (force || cursorOn != (flags & 0x1)) {
        if (cursorOn) {
            this->m_mouseModeFlags = flags | 0x1;

            if (!(this->m_flags & 0x3)) {
                g_theGxDevicePtr->CursorSetVisible(1);
            }
        } else {
            this->m_mouseModeFlags = flags & ~0x1;
            g_theGxDevicePtr->CursorSetVisible(0);
        }
    }

    this->UpdateJoystickMouseMode();
}

// INLINED
CGInputControl::~CGInputControl() {
    if (this->m_wowMouse) {
        delete this->m_wowMouse;
    }

    this->m_wowMouse = nullptr;
}

// VERIFY: e79550cf manual
// OFFSET: 0x005FD3D0
void CGInputControl::Destroy() {
    if (CGInputControl::s_inputControl) {
        delete CGInputControl::s_inputControl;
    }

    CGInputControl::s_inputControl = nullptr;
}

// VERIFY: 3a24bb1c auto
// OFFSET: 0x005FD430
void InputControlDestroy() {
    CGInputControl::Destroy();
}

// VERIFY: fae954c6 auto
// OFFSET: 0x005F95E0
void CGInputControl::UpdateHalted() {
    this->m_flags &= ~0xF0000;
}

// VERIFY: a87f5e2f auto
// OFFSET: 0x005F95F0
void CGInputControl::UpdateMoveStopped() {
    this->m_flags &= ~0x11000;
}

// VERIFY: 4f6c9ca3 manual
// OFFSET: 0x005F9600
int32_t CGInputControl::IsMouseDrag(uint32_t time) const {
    uint32_t elapsed = time - this->m_lookStartTime;

    if (static_cast<int32_t>(elapsed - 800) >= 0) {
        return 1;
    }

    if (this->m_dragAccumX >= 8.0f || this->m_dragAccumY >= 8.0f) {
        return static_cast<int32_t>(elapsed - 200) >= 0;
    }

    return 0;
}

// VERIFY: b0dd26d4 auto
// OFFSET: 0x005F96E0
float CGInputControl::GetUIVehiclePower() {
    return s_vehicleAimNormPower;
}

// VERIFY: c8871d3b auto
// OFFSET: 0x005F9750
int32_t CGInputControl::WowMouseEnabled() {
    return s_cvEnableWowMouse && s_cvEnableWowMouse->m_intValue;
}

// VERIFY: 7c49134b auto
// OFFSET: 0x005F9830
int32_t CGInputControl::IsMouseDragging() const {
    uint32_t flags = this->m_flags;

    return (flags & 0x300) && !(flags & 0x2000001);
}

// VERIFY: f9f95644 manual
// OFFSET: 0x005F9850
int32_t CGInputControl::IsIdle() const {
    uint32_t flags = this->m_flags;

    return !(flags & 0x1030) && !(flags & 0xC0) && (!(flags & 0x2000001) || !(flags & 0x300)) && (!(flags & 0x300) || (flags & 0x2000001)) && !(flags & 0x1E00000);
}

// VERIFY: de4cd81f manual
// OFFSET: 0x005F9890
XMLTree* LoadJoystickConfig(XMLNode*& config) {
    config = nullptr;

    if (s_joystick == 0xFFFFFFFF) {
        return nullptr;
    }

    char joystickName[1024];
    OsJoystickGetName(s_joystick, joystickName, sizeof(joystickName));

    void* buffer;
    uint32_t bytes;

    if (!SFile::ReadFile(nullptr, "Interface\\Joystick\\JoystickConfig.xml", &buffer, &bytes, 0, 1, nullptr)) {
        return nullptr;
    }

    XMLTree* tree = XMLTree_Load(static_cast<const char*>(buffer), bytes);
    SMemFree(buffer, __FILE__, __LINE__, 0);

    if (!tree) {
        return nullptr;
    }

    for (XMLNode* joystick = XMLTree_GetRoot(tree)->m_child; joystick; joystick = joystick->m_next) {
        if (SStrCmpI(joystick->m_name.m_str, "Joystick", STORM_MAX_STR)) {
            continue;
        }

        const char* platform = joystick->GetAttributeByName("platform");

        if (platform && SStrCmpI(platform, "windows", STORM_MAX_STR)) {
            continue;
        }

        for (XMLNode* child = joystick->m_child; child; child = child->m_next) {
            if (SStrCmpI(child->m_name.m_str, "name", STORM_MAX_STR)) {
                continue;
            }

            const char* name = child->GetAttributeByName("name");

            if (name && !SStrCmpI(name, joystickName, STORM_MAX_STR)) {
                config = joystick;
                return tree;
            }
        }
    }

    XMLTree_Free(tree);

    return nullptr;
}

// VERIFY: 62cd78e5 manual
// OFFSET: 0x005F9D20
CGUnit_C* GetVehicleUnitPtr() {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return nullptr;
    }

    if (!mover->m_vehiclePassenger || mover->m_vehiclePassenger->m_state != 3) {
        return mover;
    }

    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ mover->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicle || !vehicle->m_vehicle) {
        return mover;
    }

    VehicleSeatRec* seat = vehicle->m_vehicle->GetVehicleSeatRec(mover->m_movementData.m_transportSeat);

    if (!seat || !(seat->m_flags & 0x800)) {
        return mover;
    }

    return vehicle;
}

// VERIFY: 1144f12f auto
// OFFSET: 0x005FA060
int32_t __stdcall CGInputControl::CanControl(CGUnit_C* unit) {
    if (!unit || unit->m_unit->m_unitFieldHealth <= 0) {
        return 0;
    }

    CMoveSpline* spline = unit->m_movement->m_spline;

    if (spline && !(spline->m_flags & 0x400)) {
        return 0;
    }

    return !unit->AnimSuppressesMovement() && (!unit->IsActivePlayer() || !(static_cast<CGPlayer_C*>(unit)->m_combatModeFlags & 0x1)) && !unit->IsAlteredFormTransitionPreventingMovement();
}

// VERIFY: 44c9ffc3 manual
// OFFSET: 0x005FA6B0
int32_t CGInputControl::CameraCanTurnPlayer() const {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!(((mover->m_obj->m_type >> 4) & 1) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena())) {
        if (mover->m_unit->m_unitFieldHealth <= 0) {
            return 0;
        }

        CMoveSpline* spline = mover->m_movement->m_spline;

        if (spline && !(spline->m_flags & 0x400)) {
            return 0;
        }

        if ((mover->m_unit->m_unitFieldFlags & 0x40000) || mover->IsAlteredFormTransitionPreventingMovement() || mover->GetClientStandState()) {
            return 0;
        }

        if ((camera->m_targetGUID) != mover->m_obj->m_guid) {
            return 0;
        }
    }

    if (!(camera->m_state & 0x1)) {
        return 0;
    }

    return (this->m_flags & 0x2000001) != 0;
}

// VERIFY: 54782c60 manual
// OFFSET: 0x005FA910
void CGInputControl::SendUIVehicleAngleUpdate(float angle) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    VehicleRec* rec = mover->m_vehicle ? mover->m_vehicle->m_vehicleRec : nullptr;

    if (!rec) {
        return;
    }

    float pitchMin;
    float pitchMax;

    if (rec->m_flags & 0x40) {
        pitchMin = rec->m_pitchMin;
        pitchMax = rec->m_pitchMax;
    } else {
        pitchMin = -1.5707964f;
        pitchMax = 1.5707964f;
    }

    float range = pitchMax - pitchMin;
    float normalized = fabsf(range) <= 0.000099999997f ? 0.0f : (angle - pitchMin) / range;

    FrameScript_SignalEvent(585, "%f%f", static_cast<double>(normalized), static_cast<double>(angle));
}

// VERIFY: 08e6c5ef manual
// OFFSET: 0x005FB560
int32_t __stdcall CGInputControl::IsVehicleSeatFlagSet(uint32_t flags) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_vehiclePassenger || player->m_vehiclePassenger->m_state != 3) {
        return 0;
    }

    if (player->GetTransportGUID() != mover->m_obj->m_guid.m_guid && player->m_obj->m_guid != mover->m_obj->m_guid) {
        return 0;
    }

    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ player->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seat = vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);

    return seat && (seat->m_flags & flags);
}

// VERIFY: b6a28681 manual
// OFFSET: 0x005FD440
void CGInputControl::SetMouselookOverrideBinding(const char* key, const char* binding) {
    if (!binding) {
        MOUSELOOKBINDING* entry = this->m_mouseLookBindings.Ptr(key);

        if (entry) {
            this->m_mouseLookBindings.Delete(entry);
        }

        return;
    }

    MOUSELOOKBINDING* entry = this->m_mouseLookBindings.Ptr(key);

    if (entry) {
        if (SStrCmpI(entry->m_binding, binding, STORM_MAX_STR)) {
            SMemFree(entry->m_binding, __FILE__, __LINE__, 0);
            entry->m_binding = SStrDupA(binding, __FILE__, __LINE__);
        }
    } else {
        entry = this->m_mouseLookBindings.New(key, 0, 0);
        entry->m_binding = SStrDupA(binding, __FILE__, __LINE__);
    }
}

// VERIFY: 82de0c4b manual
// OFFSET: 0x005FA0D0
int32_t __stdcall CGInputControl::CanTurn(CGUnit_C* unit) {
    return CGInputControl::CanControl(unit) && !(unit->m_unit->m_unitFieldFlags & 0x40000) && !unit->IsVehiclePreventingTurning();
}

// VERIFY: feb80d65 manual
// OFFSET: 0x005FA110
int32_t CGInputControl::CanSyncFreeLookFacing(CGUnit_C* unit) const {
    return CGInputControl::CanControl(unit) && !(unit->m_unit->m_unitFieldFlags & 0x40000) && !unit->IsVehiclePreventingTurning() && (this->m_flags & 0x2000001) && !unit->GetClientStandState();
}

// VERIFY: 83a6aa8d manual
// OFFSET: 0x005FA420
int32_t CGInputControl::IsMouseDragMoving() const {
    int32_t result = this->IsMouseDrag(OsGetAsyncTimeMs());

    if (result) {
        return g_theGxDevicePtr->m_frameCount - this->m_lastMouseMoveFrame < 2;
    }

    return result;
}

// VERIFY: 262a3d11 manual
// OFFSET: 0x005FA790
int32_t CGInputControl::CameraCanPitchPlayer() {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (((mover->m_obj->m_type >> 4) & 1) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        return 1;
    }

    if (mover->m_unit->m_unitFieldHealth <= 0) {
        return 0;
    }

    CMoveSpline* spline = mover->m_movement->m_spline;

    if (spline && !(spline->m_flags & 0x400)) {
        return 0;
    }

    if (mover->IsAlteredFormTransitionPreventingMovement() || (mover->m_unit->m_unitFieldFlags & 0x40000) || mover->GetClientStandState()) {
        return 0;
    }

    CMovement_C* movement = mover->m_movement;

    if ((movement->m_flags2 & 0x20) && !(movement->m_flags & 0x2200000)) {
        VehicleRec* rec = mover->GetVehicleRec();

        if (!rec || !(rec->m_flags & 0x40000)) {
            return 0;
        }
    }

    if ((camera->m_targetGUID) != mover->m_obj->m_guid) {
        return 0;
    }

    return 1;
}

// VERIFY: 988e7786 manual
// OFFSET: 0x005FAC90
int32_t __stdcall CGInputControl::CanMove(CGUnit_C* unit) {
    if (!CGInputControl::CanControl(unit)) {
        return 0;
    }

    return unit->m_unit->m_unitFieldHealth > 0 && !(unit->m_movement->m_flags & 0x100A00) && unit->m_unit->m_unitFieldBytes1[0] != 7 && !unit->HasVehicleTransport();
}

// VERIFY: d0f5f494 auto
// OFFSET: 0x00568560
void __stdcall ScaleMouseDeltaByFov(float* dx, float* dy) {
    if (!CGWorldFrame::s_currentWorldFrame) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    float fovScale = camera->FOV() * 0.81851113f;
    float clamped = 1.0f;

    if (fovScale < 1.0f) {
        clamped = 0.050000001f;

        if (fovScale > 0.050000001f) {
            clamped = fovScale;
        }
    }

    float scale = 0.050000001f + clamped * 0.94999999f;
    *dx = *dx * scale;
    *dy = scale * *dy;
}

// VERIFY: 384b5dd4 manual
// OFFSET: 0x005FB510
void CGInputControl::OnPitchToAngleStop() {
    this->m_vehicleAimValid = 0;

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        CGInputControl::SendUIVehicleAngleUpdate(mover->GetPitch());
    }
}
