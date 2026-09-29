#include "PetitionVendor.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF7F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetitionVendor", &Script_ClosePetitionVendor },
    { "GetNumPetitionItems", &Script_GetNumPetitionItems },
    { "GetPetitionItemInfo", &Script_GetPetitionItemInfo },
    { "BuyPetition", &Script_BuyPetition },
    { "ClickPetitionButton", &Script_ClickPetitionButton },
    { "TurnInPetition", &Script_TurnInPetition },
    { "TurnInArenaPetition", &Script_TurnInArenaPetition },
    { "HasFilledPetition", &Script_HasFilledPetition },
};

// VERIFY: b54a5949 auto
// OFFSET: 0x005A20F0
void PetitionVendorRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
