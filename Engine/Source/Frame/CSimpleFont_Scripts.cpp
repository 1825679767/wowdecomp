#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: f86914df auto
// OFFSET: 0x004A4910
void CSimpleFont::CreateScriptMetaTable() {
    CSimpleFont::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleFont::RegisterScriptMethods);
}

// VERIFY: 954a81b3 auto
// OFFSET: 0x004A3710
void CSimpleFont::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleFont::s_metatable);
    CSimpleFont::s_metatable = LUA_NOREF;
}
