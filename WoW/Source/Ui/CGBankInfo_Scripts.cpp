#include "CGBankInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEB7C
static FrameScript_Method s_ScriptFunctions[] = {
    { "BankButtonIDToInvSlotID", &Script_BankButtonIDToInvSlotID },
    { "GetNumBankSlots", &Script_GetNumBankSlots },
    { "GetBankSlotCost", &Script_GetBankSlotCost },
    { "PurchaseSlot", &Script_PurchaseSlot },
    { "CloseBankFrame", &Script_CloseBankFrame },
};

// VERIFY: 221cb74e auto
// OFFSET: 0x0057B9E0
void BankRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
