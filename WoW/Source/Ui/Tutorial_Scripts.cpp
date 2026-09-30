#include "Tutorial.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"

// VERIFY: 9c38a03a manual
// OFFSET: 0x00530750
static int32_t Script_FlagTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: FlagTutorial(\"tutorial\")");
        return 0;
    }

    uint32_t tutorial = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (tutorial <= 59) {
        CGTutorial::FlagTutorial(tutorial);
    }

    return 0;
}

// VERIFY: d7a5866c manual
// OFFSET: 0x005308D0
static int32_t Script_TriggerTutorial(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TriggerTutorial(\"tutorial\")");
        return 0;
    }

    uint32_t tutorial = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (tutorial <= 59) {
        CGTutorial::TriggerTutorial(tutorial);
    }

    return 0;
}

// VERIFY: f778febd auto
// OFFSET: 0x005307A0
static int32_t Script_IsTutorialFlagged(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Trigger(\"tutorial\")");
        return 0;
    }

    uint32_t tutorial = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (tutorial > 59) {
        return 0;
    }

    if (IsTutorialFlagged(tutorial)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00ACC868
static FrameScript_Method s_ScriptFunctions[] = {
    { "CanResetTutorials", &Script_CanResetTutorials },
    { "FlagTutorial", &Script_FlagTutorial },
    { "IsTutorialFlagged", &Script_IsTutorialFlagged },
    { "TriggerTutorial", &Script_TriggerTutorial },
    { "ClearTutorials", &Script_ClearTutorials },
    { "ResetTutorials", &Script_ResetTutorials },
    { "GetNextCompleatedTutorial", &Script_GetNextCompleatedTutorial },
    { "GetPrevCompleatedTutorial", &Script_GetPrevCompleatedTutorial },
};

// VERIFY: 901ffa76 auto
// OFFSET: 0x005302B0
void TutorialRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
