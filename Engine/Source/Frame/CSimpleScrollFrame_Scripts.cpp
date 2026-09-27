#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: a912e40f auto
// OFFSET: 0x009727F0
void CSimpleScrollFrame::CreateScriptMetaTable() {
    CSimpleScrollFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleScrollFrame::RegisterScriptMethods);
}

// VERIFY: bef254f0 auto
// OFFSET: 0x009721E0
void CSimpleScrollFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleScrollFrame::s_metatable);
    CSimpleScrollFrame::s_metatable = LUA_NOREF;
}
