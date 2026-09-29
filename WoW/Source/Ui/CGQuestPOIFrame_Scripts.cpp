#include "CGQuestPOIFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_SetFillAlpha(lua_State* L);
int32_t Script_SetBorderAlpha(lua_State* L);
int32_t Script_SetBorderScalar(lua_State* L);
int32_t Script_DrawQuestBlob(lua_State* L);
int32_t Script_SetMergeThreshold(lua_State* L);
int32_t Script_SetNumSplinePoints(lua_State* L);
int32_t Script_UpdateMouseOverTooltip(lua_State* L);
int32_t Script_GetTooltipIndex(lua_State* L);
int32_t Script_GetNumTooltips(lua_State* L);

// VERIFY: 59c6acd1 manual
// OFFSET: 0x0058E800
static int32_t Script_EnableSmoothing(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    frame->m_smoothing = lua_toboolean(L, 2);
    return 0;
}

// VERIFY: d51ff712 manual
// OFFSET: 0x0058E850
static int32_t Script_EnableMerging(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    frame->m_merging = lua_toboolean(L, 2);
    return 0;
}

// VERIFY: b26f7e69 manual
// OFFSET: 0x0058E990
static int32_t Script_UpdateQuestPOI(lua_State* L) {
    FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType());
    return 0;
}

// VERIFY: ba309179 manual
// OFFSET: 0x0058FE50
static int32_t Script_SetFillTexture(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    frame->SetFillTexture(lua_tolstring(L, 2, nullptr));
    return 0;
}

// VERIFY: aa010914 manual
// OFFSET: 0x0058FEA0
static int32_t Script_SetBorderTexture(lua_State* L) {
    CGQuestPOIFrame* frame = static_cast<CGQuestPOIFrame*>(FrameScript_GetObjectThis(L, CGQuestPOIFrame::GetObjectType()));
    frame->SetBorderTexture(lua_tolstring(L, 2, nullptr));
    return 0;
}

// STATIC: 0x00ACF180
static FrameScript_Method s_ScriptMethods[] = {
    { "SetFillTexture", &Script_SetFillTexture },
    { "SetFillAlpha", &Script_SetFillAlpha },
    { "SetBorderTexture", &Script_SetBorderTexture },
    { "SetBorderAlpha", &Script_SetBorderAlpha },
    { "SetBorderScalar", &Script_SetBorderScalar },
    { "DrawQuestBlob", &Script_DrawQuestBlob },
    { "EnableSmoothing", &Script_EnableSmoothing },
    { "EnableMerging", &Script_EnableMerging },
    { "SetMergeThreshold", &Script_SetMergeThreshold },
    { "SetNumSplinePoints", &Script_SetNumSplinePoints },
    { "UpdateQuestPOI", &Script_UpdateQuestPOI },
    { "UpdateMouseOverTooltip", &Script_UpdateMouseOverTooltip },
    { "GetTooltipIndex", &Script_GetTooltipIndex },
    { "GetNumTooltips", &Script_GetNumTooltips },
};

// VERIFY: ae3af700 auto
// OFFSET: 0x0058DFE0
void CGQuestPOIFrame::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

// VERIFY: 5609408a auto
// OFFSET: 0x0058E000
void CGQuestPOIFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CGQuestPOIFrame::s_metatable);
    CGQuestPOIFrame::s_metatable = LUA_NOREF;
}

// VERIFY: 4ad94ed6 auto
// OFFSET: 0x0058EB00
void CGQuestPOIFrame::CreateScriptMetaTable() {
    CGQuestPOIFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CGQuestPOIFrame::RegisterScriptMethods);
}
