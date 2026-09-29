#include "PetitionInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0A60
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetition", &Script_ClosePetition },
    { "GetPetitionInfo", &Script_GetPetitionInfo },
    { "GetNumPetitionNames", &Script_GetNumPetitionNames },
    { "GetPetitionNameInfo", &Script_GetPetitionNameInfo },
    { "CanSignPetition", &Script_CanSignPetition },
    { "SignPetition", &Script_SignPetition },
    { "OfferPetition", &Script_OfferPetition },
    { "RenamePetition", &Script_RenamePetition },
};

// VERIFY: 6bda2f35 auto
// OFFSET: 0x005CEB00
void PetitionInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
