#include "CGWorldStateInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACD0D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumWorldStateUI", &Script_GetNumWorldStateUI },
    { "GetWorldStateUIInfo", &Script_GetWorldStateUIInfo },
};

// VERIFY: 30669ada auto
// OFFSET: 0x005485B0
void WorldStateInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
