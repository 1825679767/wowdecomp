#include <cstdint>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/Joystick.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "_Triage/WowMouse.h"

// STATIC: 0x00AD192C
uint32_t s_joystick = 0xFFFFFFFF;

// STATIC: 0x00AD1930
int32_t s_joystickMouseEnabled = 1;

// STATIC: 0x00C24954
CGInputControl* CGInputControl::s_inputControl;

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
