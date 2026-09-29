#include "DuelInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0AC4
static FrameScript_Method s_ScriptFunctions[] = {
    { "StartDuel", &Script_StartDuel },
    { "AcceptDuel", &Script_AcceptDuel },
    { "CancelDuel", &Script_CancelDuel },
};

// VERIFY: edf0fec6 auto
// OFFSET: 0x005CF9C0
void DuelInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
