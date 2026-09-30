#include "CurrencyTypes.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// VERIFY: 051f8586 auto
// OFFSET: 0x005AFD10
static int32_t Script_GetCurrencyListSize(lua_State* L) {
    lua_pushnumber(L, CGCurrencyTypes::m_listSize);
    return 1;
}

// STATIC: 0x00ACFC34
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetCurrencyListSize", &Script_GetCurrencyListSize },
    { "GetCurrencyListInfo", &Script_GetCurrencyListInfo },
    { "ExpandCurrencyList", &Script_ExpandCurrencyList },
    { "SetCurrencyUnused", &Script_SetCurrencyUnused },
    { "SetCurrencyBackpack", &Script_SetCurrencyBackpack },
    { "GetBackpackCurrencyInfo", &Script_GetBackpackCurrencyInfo },
};

// VERIFY: 5ce58e3b auto
// OFFSET: 0x005AFD40
void CurrencyTypesRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
