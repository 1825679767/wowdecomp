#include "ActionBarFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/UIMacros.h"

// STATIC: 0x00ACF9E0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetActionInfo", &Script_GetActionInfo },
    { "GetActionTexture", &Script_GetActionTexture },
    { "GetActionCount", &Script_GetActionCount },
    { "GetActionCooldown", &Script_GetActionCooldown },
    { "GetActionAutocast", &Script_GetActionAutocast },
    { "GetActionText", &Script_GetActionText },
    { "HasAction", &Script_HasAction },
    { "UseAction", &Script_UseAction },
    { "PickupAction", &Script_PickupAction },
    { "PlaceAction", &Script_PlaceAction },
    { "IsAttackAction", &Script_IsAttackAction },
    { "IsCurrentAction", &Script_IsCurrentAction },
    { "IsAutoRepeatAction", &Script_IsAutoRepeatAction },
    { "IsUsableAction", &Script_IsUsableAction },
    { "IsConsumableAction", &Script_IsConsumableAction },
    { "IsStackableAction", &Script_IsStackableAction },
    { "IsEquippedAction", &Script_IsEquippedAction },
    { "ActionHasRange", &Script_ActionHasRange },
    { "IsActionInRange", &Script_IsActionInRange },
    { "GetBonusBarOffset", &Script_GetBonusBarOffset },
    { "GetMultiCastBarOffset", &Script_GetMultiCastBarOffset },
    { "ChangeActionBarPage", &Script_ChangeActionBarPage },
    { "GetActionBarPage", &Script_GetActionBarPage },
    { "GetActionBarToggles", &Script_GetActionBarToggles },
    { "SetActionBarToggles", &Script_SetActionBarToggles },
    { "IsPossessBarVisible", &Script_IsPossessBarVisible },
    { "GetMultiCastTotemSpells", &Script_GetMultiCastTotemSpells },
    { "SetMultiCastSpell", &Script_SetMultiCastSpell },
};

// VERIFY: 3766f2a3 auto
// OFFSET: 0x005A8020
void ActionBarRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: b655df32 manual
// OFFSET: 0x005A7E60
int32_t Script_IsUsableAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsUsableAction(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (slot >= 0 && slot < sizeof(CGActionBar::m_slotActions) / sizeof(CGActionBar::m_slotActions[0]) && CGActionBar::m_slotUsable[slot]) {
        if (CGActionBar::m_slotNoMana[slot]) {
            lua_pushnil(L);
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnumber(L, 1.0);
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// VERIFY: 7f585b01 auto
// OFFSET: 0x005A8220
int32_t Script_HasAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: HasAction(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));

    if (CGActionBar::HasAction(slot - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 6a7a8f81 manual
// OFFSET: 0x005A8F10
int32_t Script_GetActionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionInfo(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t action = CGActionBar::m_slotActions[slot - 1];

    if (action && !(action & 0xF0000000)) {
        int32_t petSpell = 0;
        int32_t spellId = CGActionBar::GetSpell(slot - 1, petSpell);
        SpellRec spell;
        g_spellDB.GetRecord(spellId, &spell);

        if (SpellIsCallCompanion(&spell)) {
            int32_t type = CGSpellBook::GetCompanionType(&spell);
            lua_pushstring(L, "companion");
            lua_pushnumber(L, CGSpellBook::FindCompanionSlotByID(spellId, type) + 1);

            if (type == 0) {
                lua_pushstring(L, "CRITTER");
            } else if (type == 1) {
                lua_pushstring(L, "MOUNT");
            } else {
                lua_pushstring(L, "UNKNOWN");
            }
        } else {
            lua_pushstring(L, "spell");
            lua_pushnumber(L, GetSpellSlot(spellId, petSpell) + 1);
            lua_pushstring(L, petSpell ? "pet" : "spell");
        }

        lua_pushnumber(L, spellId);
        return 4;
    }

    if (action && (action & 0xF0000000) == 0x10000000) {
        int32_t spellId = CGActionBar::GetPetActionSpell(slot - 1);
        lua_pushstring(L, "spell");
        lua_pushnumber(L, GetSpellSlot(spellId, 1) + 1);
        lua_pushstring(L, "pet");
        lua_pushnumber(L, spellId);
        return 4;
    }

    if (CGActionBar::IsItem(slot - 1)) {
        lua_pushstring(L, "item");
        lua_pushnumber(L, CGActionBar::GetItem(slot - 1));
        return 2;
    }

    if (CGActionBar::IsMacro(slot - 1)) {
        lua_pushstring(L, "macro");
        lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(CGActionBar::GetMacroID(slot - 1)) + 1);
        return 2;
    }

    if (!CGActionBar::IsEquipmentSet(slot - 1)) {
        return 0;
    }

    const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGActionBar::GetEquipmentSetID(slot - 1));

    if (!set) {
        return 0;
    }

    lua_pushstring(L, "equipmentset");
    lua_pushstring(L, set->m_name);
    return 2;
}

// VERIFY: 93779dc5 manual
// OFFSET: 0x005A91C0
int32_t Script_GetActionCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionCooldown(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t start;
    uint32_t duration;
    uint32_t enable = 0;
    CGActionBar::GetCooldown(slot - 1, start, duration, enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// VERIFY: 8e30b2d9 auto
// OFFSET: 0x005A9B30
int32_t Script_GetActionTexture(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionTexture(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    const char* texture = CGActionBar::GetTexture(slot - 1);

    if (texture) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: a0aeec6d auto
// OFFSET: 0x005AAD40
int32_t Script_IsCurrentAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsCurrentAction(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));

    if (CGActionBar::IsCurrentAction(slot - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 0e44e750 manual
// OFFSET: 0x005AC000
int32_t Script_UseAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: UseAction(slot, [, target] [, button])");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    WOWGUID target;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 2, nullptr), target, true)) {
        CGActionBar::UseAction(slot - 1, target, lua_tolstring(L, 3, nullptr));
    }

    return 0;
}

// VERIFY: 0b449373 auto
// OFFSET: 0x005A7D90
int32_t Script_GetActionText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionText(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t action = CGActionBar::m_slotActions[slot - 1];

    if (action) {
        if ((action & 0xF0000000) == 0x40000000) {
            MACRONODE* macro = CGUIMacros::GetMacroByID(action & 0xBFFFFFFF);

            if (macro) {
                lua_pushstring(L, macro->m_name);
                return 1;
            }
        } else if ((action & 0xF0000000) == 0x20000000) {
            const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGActionBar::GetEquipmentSetID(slot - 1));

            if (set) {
                lua_pushstring(L, set->m_name);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: d3957535 manual
// OFFSET: 0x005A7F20
int32_t Script_GetBonusBarOffset(lua_State* L) {
    lua_pushnumber(L, CGActionBar::m_bonusBarOffset);
    return 1;
}
