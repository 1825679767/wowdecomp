#include "UIBindings.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/Types.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACDE38
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumBindings", &Script_GetNumBindings },
    { "GetBinding", &Script_GetBinding },
    { "SetBinding", &Script_SetBinding },
    { "SetBindingSpell", &Script_SetBindingSpell },
    { "SetBindingItem", &Script_SetBindingItem },
    { "SetBindingMacro", &Script_SetBindingMacro },
    { "SetBindingClick", &Script_SetBindingClick },
    { "SetOverrideBinding", &Script_SetOverrideBinding },
    { "SetOverrideBindingSpell", &Script_SetOverrideBindingSpell },
    { "SetOverrideBindingItem", &Script_SetOverrideBindingItem },
    { "SetOverrideBindingMacro", &Script_SetOverrideBindingMacro },
    { "SetOverrideBindingClick", &Script_SetOverrideBindingClick },
    { "ClearOverrideBindings", &Script_ClearOverrideBindings },
    { "GetBindingKey", &Script_GetBindingKey },
    { "GetBindingAction", &Script_GetBindingAction },
    { "GetBindingByKey", &Script_GetBindingByKey },
    { "RunBinding", &Script_RunBinding },
    { "GetCurrentBindingSet", &Script_GetCurrentBindingSet },
    { "LoadBindings", &Script_LoadBindings },
    { "SaveBindings", &Script_SaveBindings },
    { "GetNumModifiedClickActions", &Script_GetNumModifiedClickActions },
    { "GetModifiedClickAction", &Script_GetModifiedClickAction },
    { "SetModifiedClick", &Script_SetModifiedClick },
    { "GetModifiedClick", &Script_GetModifiedClick },
    { "IsModifiedClick", &Script_IsModifiedClick },
    { "GetClickFrame", &Script_GetClickFrame },
};

// VERIFY: 8e440f04 auto
// OFFSET: 0x0055DC90
void UIBindingsRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: 28f45f28 auto
// OFFSET: 0x0055E9B0
int32_t Script_GetBindingKey(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetBindingKey(\"COMMAND\"[, mode])");
    }

    BINDING_MODE mode = BINDING_MODE_ACTIVE;

    if (lua_isnumber(L, 2)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

        if (index <= BINDING_MODE_3) {
            mode = static_cast<BINDING_MODE>(index);
        }
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;
    int32_t count = 0;
    const char* command = lua_tolstring(L, 1, nullptr);

    for (const char* key = bindings->GetCommandKey(mode, command, 0); key; key = bindings->GetCommandKey(mode, command, count)) {
        count++;
        lua_pushstring(L, key);
    }

    return count;
}
