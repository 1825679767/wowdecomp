#include "CGMinigameInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD05D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetMinigameType", &Script_GetMinigameType },
    { "MakeMinigameMove", &Script_MakeMinigameMove },
    { "GetMinigameState", &Script_GetMinigameState },
};

// VERIFY: dedde433 auto
// OFFSET: 0x005C5610
void MinigameInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: 380c401b auto
// OFFSET: 0x005C5640
void MinigameInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
