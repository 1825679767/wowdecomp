#include "CGWorldStateInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: f57c55c3 manual
// OFFSET: 0x00548720
static int32_t Script_GetNumWorldStateUI(lua_State* L) {
    lua_pushnumber(L, CGWorldStateInfo::m_worldStateUI.m_count);
    return 1;
}

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
