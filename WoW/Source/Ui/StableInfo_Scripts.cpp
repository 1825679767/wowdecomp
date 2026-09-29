#include "StableInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF768
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetStables", &Script_ClosePetStables },
    { "StablePet", &Script_StablePet },
    { "UnstablePet", &Script_UnstablePet },
    { "BuyStableSlot", &Script_BuyStableSlot },
    { "GetNumStablePets", &Script_GetNumStablePets },
    { "GetNumStableSlots", &Script_GetNumStableSlots },
    { "GetStablePetInfo", &Script_GetStablePetInfo },
    { "GetNextStableSlotCost", &Script_GetNextStableSlotCost },
    { "ClickStablePet", &Script_ClickStablePet },
    { "PickupStablePet", &Script_PickupStablePet },
    { "GetSelectedStablePet", &Script_GetSelectedStablePet },
    { "SetPetStablePaperdoll", &Script_SetPetStablePaperdoll },
    { "GetStablePetFoodTypes", &Script_GetStablePetFoodTypes },
    { "IsAtStableMaster", &Script_IsAtStableMaster },
};

// VERIFY: 11c70b44 auto
// OFFSET: 0x005A10D0
void StableInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
