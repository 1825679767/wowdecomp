#include "MinimapFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_SetMaskTexture(lua_State* L);
int32_t Script_SetIconTexture(lua_State* L);
int32_t Script_SetBlipTexture(lua_State* L);
int32_t Script_SetClassBlipTexture(lua_State* L);
int32_t Script_SetPOIArrowTexture(lua_State* L);
int32_t Script_SetStaticPOIArrowTexture(lua_State* L);
int32_t Script_SetCorpsePOIArrowTexture(lua_State* L);
int32_t Script_SetPlayerTexture(lua_State* L);
int32_t Script_SetPlayerTextureHeight(lua_State* L);
int32_t Script_SetPlayerTextureWidth(lua_State* L);
int32_t Script_GetZoomLevels(lua_State* L);
int32_t Script_GetZoom(lua_State* L);
int32_t Script_SetZoom(lua_State* L);
int32_t Script_PingLocation(lua_State* L);
int32_t Script_GetPingPosition(lua_State* L);

// STATIC: 0x00ACEBB8
static FrameScript_Method s_ScriptMethods[] = {
    { "SetMaskTexture", &Script_SetMaskTexture },
    { "SetIconTexture", &Script_SetIconTexture },
    { "SetBlipTexture", &Script_SetBlipTexture },
    { "SetClassBlipTexture", &Script_SetClassBlipTexture },
    { "SetPOIArrowTexture", &Script_SetPOIArrowTexture },
    { "SetStaticPOIArrowTexture", &Script_SetStaticPOIArrowTexture },
    { "SetCorpsePOIArrowTexture", &Script_SetCorpsePOIArrowTexture },
    { "SetPlayerTexture", &Script_SetPlayerTexture },
    { "SetPlayerTextureHeight", &Script_SetPlayerTextureHeight },
    { "SetPlayerTextureWidth", &Script_SetPlayerTextureWidth },
    { "GetZoomLevels", &Script_GetZoomLevels },
    { "GetZoom", &Script_GetZoom },
    { "SetZoom", &Script_SetZoom },
    { "PingLocation", &Script_PingLocation },
    { "GetPingPosition", &Script_GetPingPosition },
};

// VERIFY: acea5034 auto
// OFFSET: 0x0057C040
void CGMinimapFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// VERIFY: 756048df auto
// OFFSET: 0x0057C060
void CGMinimapFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGMinimapFrame::s_metatable);
    CGMinimapFrame::s_metatable = LUA_NOREF;
}

// STATIC: 0x00ACEC30
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumTrackingTypes", &Script_GetNumTrackingTypes },
    { "GetTrackingInfo", &Script_GetTrackingInfo },
    { "SetTracking", &Script_SetTracking },
    { "GetTrackingTexture", &Script_GetTrackingTexture },
};

// VERIFY: 41ea4a09 auto
// OFFSET: 0x0057C090
void MinimapRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: 6485707d auto
// OFFSET: 0x0057E340
void CGMinimapFrame::CreateScriptMetaTable() {
    CGMinimapFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGMinimapFrame::RegisterScriptMethods);
}
