#include "EquipmentManager.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
