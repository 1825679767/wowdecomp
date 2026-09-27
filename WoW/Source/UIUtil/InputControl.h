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

struct MOUSELOOKBINDING : public TSHashObject<MOUSELOOKBINDING, HASHKEY_STRI> {
};

class CGInputControl {
    public:
    static CGInputControl* s_inputControl;

    static void Destroy();
    static CGInputControl* GetActive();

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
    void SetMouseCVarHandler(bool enable);
    void UpdateJoystickMouseMode();
    void UpdateMouseMode(int32_t force);
};

extern CVar* s_cvCinematicJoystick;
extern CVar* s_cvEnableWowMouse;
extern uint32_t s_joystick;
extern int32_t s_joystickMouseEnabled;

void InputControlDestroy();
void InputControlInitialize();
bool JoystickCallback(CVar* var, const char* oldValue, const char* value, void* arg);
XMLTree* LoadJoystickConfig(XMLNode*& config);
bool WowMouseCVarCallback(CVar* var, const char* oldValue, const char* value, void* arg);

#endif
