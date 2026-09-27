#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 31c4b9cf auto
// OFFSET: 0x009752A0
void CSimpleHTML::CreateScriptMetaTable() {
    CSimpleHTML::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleHTML::RegisterScriptMethods);
}

// VERIFY: 2f8ea65e auto
// OFFSET: 0x009749E0
void CSimpleHTML::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleHTML::s_metatable);
    CSimpleHTML::s_metatable = LUA_NOREF;
}
