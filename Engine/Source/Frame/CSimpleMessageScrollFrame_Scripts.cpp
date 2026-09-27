#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: b3f5711e auto
// OFFSET: 0x00973D00
void CSimpleMessageScrollFrame::CreateScriptMetaTable() {
    CSimpleMessageScrollFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleMessageScrollFrame::RegisterScriptMethods);
}

// VERIFY: be4532f0 auto
// OFFSET: 0x00972830
void CSimpleMessageScrollFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleMessageScrollFrame::s_metatable);
    CSimpleMessageScrollFrame::s_metatable = LUA_NOREF;
}
