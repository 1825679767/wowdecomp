#ifndef WOW_SOURCE_UIUTIL_INPUTCONTROL_H
#define WOW_SOURCE_UIUTIL_INPUTCONTROL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/UIUtil/Types.h"

struct CVar;
class XMLNode;
struct HASHKEY_STRI;
struct MOUSELOOKBINDING;
struct XMLTree;
class WowMouse;
class CGUnit_C;

struct MOUSELOOKBINDING : public TSHashObject<MOUSELOOKBINDING, HASHKEY_STRI> {
    /* 0018 */ char* m_binding;
};

class CGInputControl {
    public:
    static CGInputControl* s_inputControl;

    static int32_t CameraCanPitchPlayer();
    static int32_t __stdcall CanControl(CGUnit_C* unit);
    static int32_t __stdcall CanMove(CGUnit_C* unit);
    static int32_t __stdcall CanTurn(CGUnit_C* unit);
    static void Destroy();
    static CGInputControl* GetActive();
    static float GetUIVehiclePower();
    static int32_t __stdcall IsVehicleSeatFlagSet(uint32_t flags);
    static void SendUIVehicleAngleUpdate(float angle);
    static int32_t WowMouseEnabled();

    /* 0000 */ uint32_t m_time;
    /* 0004 */ uint32_t m_flags;
    /* 0008 */ float m_dragAccumX;
    /* 000C */ float m_dragAccumY;
    /* 0010 */ uint32_t m_lastMouseMoveFrame;
    /* 0014 */ uint32_t m_lookStartTime;
    /* 0018 */ PENDING_DEFAULT_ACTION m_pendingDefaultAction;
    /* 001C */ TSHashTable<MOUSELOOKBINDING, HASHKEY_STRI> m_mouseLookBindings;
    /* 0044 */ uint32_t m_facingOverrideActive;
    /* 0048 */ float m_facingOverride;
    /* 004C */ uint32_t m_vehicleAimValid;
    /* 0050 */ float m_vehicleAim;
    /* 0054 */ uint32_t m_freeLookFacingSynced;
    /* 0058 */ uint32_t m_mouseModeFlags;
    /* 005C */ uint32_t m_forceCursorOn;
    /* 0060 */ uint32_t m_joystickMouseFlags;
    /* 0064 */ float m_joystickLookX;
    /* 0068 */ float m_joystickLookY;
    /* 006C */ WowMouse* m_wowMouse;

    CGInputControl();
    ~CGInputControl();
    int32_t CameraCanTurnPlayer() const;
    int32_t CanSyncFreeLookFacing(CGUnit_C* unit) const;
    int32_t IsIdle() const;
    int32_t IsMouseDrag(uint32_t time) const;
    int32_t IsMouseDragMoving() const;
    int32_t IsMouseDragging() const;
    void OnPitchToAngleStop();
    void SetMouselookOverrideBinding(const char* key, const char* binding);
    void SetMouseCVarHandler(bool enable);
    void UpdateJoystickMouseMode();
    void UpdateHalted();
    void UpdateMouseMode(int32_t force);
    void UpdateMoveStopped();
    void ToggleControlBit(uint32_t bit, int32_t enable, uint32_t time, int32_t a5);
};

extern CVar* s_cvCinematicJoystick;
extern CVar* s_cvEnableWowMouse;
extern uint32_t s_joystick;
extern int32_t s_joystickMouseEnabled;

CGUnit_C* GetVehicleUnitPtr();
void InputControlDestroy();
void InputControlInitialize();
void __stdcall ScaleMouseDeltaByFov(float* dx, float* dy);
bool JoystickCallback(CVar* var, const char* oldValue, const char* value, void* arg);
XMLTree* LoadJoystickConfig(XMLNode*& config);
bool WowMouseCVarCallback(CVar* var, const char* oldValue, const char* value, void* arg);

#endif
