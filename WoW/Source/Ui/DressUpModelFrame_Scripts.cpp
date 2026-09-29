#include "DressUpModelFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_Undress(lua_State* L);
int32_t Script_Dress(lua_State* L);
int32_t Script_TryOn(lua_State* L);

// STATIC: 0x00ACF550
static FrameScript_Method s_ScriptMethods[] = {
    { "Undress", &Script_Undress },
    { "Dress", &Script_Dress },
    { "TryOn", &Script_TryOn },
};

// VERIFY: 9ed91f35 auto
// OFFSET: 0x00597CC0
void CGDressUpModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// VERIFY: d9ce6dd3 auto
// OFFSET: 0x00597CE0
void CGDressUpModelFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGDressUpModelFrame::s_metatable);
    CGDressUpModelFrame::s_metatable = LUA_NOREF;
}

// VERIFY: 067c0bb2 auto
// OFFSET: 0x00597F20
void CGDressUpModelFrame::CreateScriptMetaTable() {
    CGDressUpModelFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGDressUpModelFrame::RegisterScriptMethods);
}
