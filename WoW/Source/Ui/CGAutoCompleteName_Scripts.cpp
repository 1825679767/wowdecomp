#include "CGAutoCompleteName.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEB50
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetAutoCompleteResults", &Script_GetAutoCompleteResults },
    { "GetAutoCompletePresenceID", &Script_GetAutoCompletePresenceID },
};

// VERIFY: 9bd82136 auto
// OFFSET: 0x0057ADB0
void AutoCompleteRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
