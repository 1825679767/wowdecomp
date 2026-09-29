#include "LootFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEE28
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLootPortrait", &Script_SetLootPortrait },
    { "GetNumLootItems", &Script_GetNumLootItems },
    { "GetLootSlotInfo", &Script_GetLootSlotInfo },
    { "GetLootSlotLink", &Script_GetLootSlotLink },
    { "LootSlotIsItem", &Script_LootSlotIsItem },
    { "LootSlotIsCoin", &Script_LootSlotIsCoin },
    { "LootSlot", &Script_LootSlot },
    { "ConfirmLootSlot", &Script_ConfirmLootSlot },
    { "CloseLoot", &Script_CloseLoot },
    { "IsFishingLoot", &Script_IsFishingLoot },
    { "GetMasterLootCandidate", &Script_GetMasterLootCandidate },
    { "GiveMasterLoot", &Script_GiveMasterLoot },
    { "GetLootRollItemInfo", &Script_GetLootRollItemInfo },
    { "GetLootRollItemLink", &Script_GetLootRollItemLink },
    { "GetLootRollTimeLeft", &Script_GetLootRollTimeLeft },
    { "RollOnLoot", &Script_RollOnLoot },
    { "ConfirmLootRoll", &Script_ConfirmLootRoll },
};

// VERIFY: 25122094 auto
// OFFSET: 0x00588E60
void LootInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
