#include "DanceStudio.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACE834
static FrameScript_Method s_ScriptFunctions[] = {
    { "PlayDance", &Script_PlayDance },
};

// VERIFY: 4f09f3b4 auto
// OFFSET: 0x00575820
void CGDanceStudio::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
