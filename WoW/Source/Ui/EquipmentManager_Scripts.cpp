#include "EquipmentManager.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// VERIFY: 53148363 manual
// OFFSET: 0x005AD770
static int32_t Script_EquipmentManagerIgnoreSlotForSave(lua_State* L) {
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 18) {
        luaL_error(L, "Usage: EquipmentManagerIgnoreSlotForSave(slot)");
        return 0;
    }

    CGEquipmentManager::m_ignoredSlots[slot] = 1;
    return 0;
}

// VERIFY: 7ab2e1ba manual
// OFFSET: 0x005AD830
static int32_t Script_EquipmentManagerUnignoreSlotForSave(lua_State* L) {
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 18) {
        luaL_error(L, "Usage: UpdatePendingEquipmentSetSlot(slot)");
        return 0;
    }

    CGEquipmentManager::m_ignoredSlots[slot] = 0;
    return 0;
}

// VERIFY: 8b0d091f manual
// OFFSET: 0x005AD7C0
static int32_t Script_EquipmentManagerIsSlotIgnoredForSave(lua_State* L) {
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 18) {
        luaL_error(L, "Usage: EquipmentSetIsSlotIgnoredForSave(slot)");
        return 0;
    }

    lua_pushboolean(L, CGEquipmentManager::m_ignoredSlots[slot] != 0);
    return 1;
}

// VERIFY: 9e4abd85 manual
// OFFSET: 0x005AD880
static int32_t Script_CanUseEquipmentSets(lua_State* L) {
    lua_pushnumber(L, CGEquipmentManager::CanUseEquipmentSets(true));
    return 1;
}

// STATIC: 0x00ACFB78
static FrameScript_Method s_ScriptFunctions[] = {
    { "SaveEquipmentSet", &Script_SaveEquipmentSet },
    { "DeleteEquipmentSet", &Script_DeleteEquipmentSet },
    { "RenameEquipmentSet", &Script_RenameEquipmentSet },
    { "EquipmentManagerIgnoreSlotForSave", &Script_EquipmentManagerIgnoreSlotForSave },
    { "EquipmentManagerIsSlotIgnoredForSave", &Script_EquipmentManagerIsSlotIgnoredForSave },
    { "EquipmentManagerClearIgnoredSlotsForSave", &Script_EquipmentManagerClearIgnoredSlotsForSave },
    { "EquipmentManagerUnignoreSlotForSave", &Script_EquipmentManagerUnignoreSlotForSave },
    { "GetEquipmentSetLocations", &Script_GetEquipmentSetLocations },
    { "GetEquipmentSetItemIDs", &Script_GetEquipmentSetItemIDs },
    { "GetNumEquipmentSets", &Script_GetNumEquipmentSets },
    { "GetEquipmentSetInfo", &Script_GetEquipmentSetInfo },
    { "GetEquipmentSetInfoByName", &Script_GetEquipmentSetInfoByName },
    { "EquipmentSetContainsLockedItems", &Script_EquipmentSetContainsLockedItems },
    { "PickupEquipmentSetByName", &Script_PickupEquipmentSetByName },
    { "PickupEquipmentSet", &Script_PickupEquipmentSet },
    { "UseEquipmentSet", &Script_UseEquipmentSet },
    { "CanUseEquipmentSets", &Script_CanUseEquipmentSets },
};

// VERIFY: 2ca3f5dd auto
// OFFSET: 0x005AD8F0
void EquipmentManagerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
