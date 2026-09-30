#include "UIMacros.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 3dacc7a5 auto
// OFFSET: 0x00564B90
static int32_t Script_StopMacro(lua_State* L) {
    if (CGGameUI::CanPerformAction(3)) {
        CGUIMacros::m_macroButton = nullptr;
        CGUIMacros::m_macroRunning = nullptr;
    }

    return 0;
}

// VERIFY: ca803391 auto
// OFFSET: 0x00564C70
static int32_t Script_GetRunningMacroButton(lua_State* L) {
    lua_pushstring(L, CGUIMacros::m_macroRunning);
    return 1;
}

// VERIFY: 42a0c983 manual
// OFFSET: 0x00564BB0
static int32_t Script_GetNumMacros(lua_State* L) {
    lua_pushnumber(L, CGUIMacros::m_numMacros);
    lua_pushnumber(L, CGUIMacros::m_numCharacterMacros);
    return 2;
}

// VERIFY: c78f099b auto
// OFFSET: 0x00564C10
static int32_t Script_GetRunningMacro(lua_State* L) {
    uint32_t id;

    if (CGUIMacros::m_macroButton && (id = CGUIMacros::m_macroButton->m_id)) {
        lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(id) + 1);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00ACE1F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "SecureCmdOptionParse", &Script_SecureCmdOptionParse },
    { "RunMacro", &Script_RunMacro },
    { "RunMacroText", &Script_RunMacroText },
    { "StopMacro", &Script_StopMacro },
    { "CreateMacro", &Script_CreateMacro },
    { "GetNumMacros", &Script_GetNumMacros },
    { "GetMacroInfo", &Script_GetMacroInfo },
    { "GetMacroBody", &Script_GetMacroBody },
    { "DeleteMacro", &Script_DeleteMacro },
    { "EditMacro", &Script_EditMacro },
    { "SetMacroItem", &Script_SetMacroItem },
    { "GetMacroItem", &Script_GetMacroItem },
    { "SetMacroSpell", &Script_SetMacroSpell },
    { "GetMacroSpell", &Script_GetMacroSpell },
    { "GetNumMacroIcons", &Script_GetNumMacroIcons },
    { "GetNumMacroItemIcons", &Script_GetNumMacroItemIcons },
    { "GetMacroIconInfo", &Script_GetMacroIconInfo },
    { "GetMacroItemIconInfo", &Script_GetMacroItemIconInfo },
    { "PickupMacro", &Script_PickupMacro },
    { "GetMacroIndexByName", &Script_GetMacroIndexByName },
    { "GetRunningMacro", &Script_GetRunningMacro },
    { "GetRunningMacroButton", &Script_GetRunningMacroButton },
};

// VERIFY: 72f0afdb auto
// OFFSET: 0x00564C90
void UIMacrosRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
