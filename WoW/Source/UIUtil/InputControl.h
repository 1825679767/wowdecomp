#ifndef WOW_SOURCE_UIUTIL_INPUTCONTROL_H
#define WOW_SOURCE_UIUTIL_INPUTCONTROL_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/UIUtil/Types.h"

struct HASHKEY_STRI;
struct MOUSELOOKBINDING;

struct MOUSELOOKBINDING : public TSHashObject<MOUSELOOKBINDING, HASHKEY_STRI> {
};

class CGInputControl {
    public:
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
    /* 006C */ void* m_wowMouse;
};

#endif
