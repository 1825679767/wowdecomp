#include "TabardModelFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_InitializeTabardColors(lua_State* L);
int32_t Script_Save(lua_State* L);
int32_t Script_CycleVariation(lua_State* L);
int32_t Script_GetUpperBackgroundFileName(lua_State* L);
int32_t Script_GetLowerBackgroundFileName(lua_State* L);
int32_t Script_GetUpperEmblemFileName(lua_State* L);
int32_t Script_GetLowerEmblemFileName(lua_State* L);
int32_t Script_GetUpperEmblemTexture(lua_State* L);
int32_t Script_GetLowerEmblemTexture(lua_State* L);
int32_t Script_CanSaveTabardNow(lua_State* L);

// STATIC: 0x00ACF588
static FrameScript_Method s_ScriptMethods[] = {
    { "InitializeTabardColors", &Script_InitializeTabardColors },
    { "Save", &Script_Save },
    { "CycleVariation", &Script_CycleVariation },
    { "GetUpperBackgroundFileName", &Script_GetUpperBackgroundFileName },
    { "GetLowerBackgroundFileName", &Script_GetLowerBackgroundFileName },
    { "GetUpperEmblemFileName", &Script_GetUpperEmblemFileName },
    { "GetLowerEmblemFileName", &Script_GetLowerEmblemFileName },
    { "GetUpperEmblemTexture", &Script_GetUpperEmblemTexture },
    { "GetLowerEmblemTexture", &Script_GetLowerEmblemTexture },
    { "CanSaveTabardNow", &Script_CanSaveTabardNow },
};

// VERIFY: ff1f59bb auto
// OFFSET: 0x00598FE0
void CGTabardModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// VERIFY: 5cd46383 auto
// OFFSET: 0x00599000
void CGTabardModelFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGTabardModelFrame::s_metatable);
    CGTabardModelFrame::s_metatable = LUA_NOREF;
}

// VERIFY: 07bd4eda auto
// OFFSET: 0x00599390
void CGTabardModelFrame::CreateScriptMetaTable() {
    CGTabardModelFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGTabardModelFrame::RegisterScriptMethods);
}
