#ifndef WOW_SOURCE_UI_UIBINDINGS_H
#define WOW_SOURCE_UI_UIBINDINGS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Ui/KEYBINDING.h"
#include "WoW/Source/Ui/MODIFIEDCLICK.h"

struct HASHKEY_STRI;
class KEYBINDING;
struct KEYCOMMAND;
class MODIFIEDCLICK;
struct OVERRIDEKEYBINDING;

struct KEYCOMMAND : public TSHashObject<KEYCOMMAND, HASHKEY_STRI> {
    /* 0018 */ int32_t m_index;
    /* 001C */ int32_t m_function;
    /* 0020 */ int32_t m_runOnUp;
    /* 0024 */ int32_t m_pressure;
    /* 0028 */ int32_t m_angle;
};

struct OVERRIDEKEYBINDING {
    /* 0000 */ uint8_t unk_0000[1];
};

class CGUIBindings {
    public:
    /* 0000 */ int32_t m_numCommands;
    /* 0004 */ int32_t m_numHiddenCommands;
    /* 0008 */ int32_t m_numModifiedClicks;
    /* 000C */ TSHashTable<KEYCOMMAND, HASHKEY_STRI> m_commands;
    /* 0034 */ TSHashTable<KEYBINDING, HASHKEY_STRI> m_bindings [4];
    /* 00D4 */ TSHashTable<OVERRIDEKEYBINDING, HASHKEY_STRI> m_overrideBindings;
    /* 00FC */ TSHashTable<MODIFIEDCLICK, HASHKEY_STRI> m_modifiedClicks;
    /* 0124 */ int32_t m_saveSet;
    /* 0128 */ uint32_t m_dirtySets;
    /* 012C */ int32_t m_keyStateOverride;
    /* 0130 */ uint32_t m_heldModifiers;
    /* 0134 */ uint32_t m_bindingModeMask;
    /* 0138 */ uint32_t m_bindingModeToggles;
};

#endif
