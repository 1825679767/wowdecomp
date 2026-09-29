#include "CharacterModelBase.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_ModelSetUnit(lua_State* L);
int32_t Script_SetCreature(lua_State* L);
int32_t Script_RefreshUnit(lua_State* L);
int32_t Script_SetRotation(lua_State* L);

// STATIC: 0x00ACF514
static FrameScript_Method s_ScriptMethods[] = {
    { "SetUnit", &Script_ModelSetUnit },
    { "SetCreature", &Script_SetCreature },
    { "RefreshUnit", &Script_RefreshUnit },
    { "SetRotation", &Script_SetRotation },
};

// VERIFY: 477f0f97 auto
// OFFSET: 0x00597230
void CGCharacterModelBase::RegisterScriptMethods(lua_State* L) {
    CSimpleModel::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// VERIFY: 6ff99da1 auto
// OFFSET: 0x00597250
void CGCharacterModelBase::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGCharacterModelBase::s_metatable);
    CGCharacterModelBase::s_metatable = LUA_NOREF;
}

// VERIFY: 71945220 auto
// OFFSET: 0x00597A80
void CGCharacterModelBase::CreateScriptMetaTable() {
    CGCharacterModelBase::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGCharacterModelBase::RegisterScriptMethods);
}
