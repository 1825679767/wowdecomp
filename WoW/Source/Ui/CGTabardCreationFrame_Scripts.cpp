#include "CGTabardCreationFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// VERIFY: 5a471781 manual
// OFFSET: 0x00598DD0
static int32_t Script_GetTabardCreationCost(lua_State* L) {
    lua_pushnumber(L, GuildGetTabardCost());
    return 1;
}

// STATIC: 0x00ACF56C
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTabardCreation", &Script_CloseTabardCreation },
    { "GetTabardCreationCost", &Script_GetTabardCreationCost },
};

// VERIFY: c7843cca auto
// OFFSET: 0x00598E10
void TabardCreationRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
