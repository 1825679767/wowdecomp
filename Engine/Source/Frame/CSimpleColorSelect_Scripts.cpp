#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: f7132f5d auto
// OFFSET: 0x00971190
void CSimpleColorSelect::CreateScriptMetaTable() {
    CSimpleColorSelect::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleColorSelect::RegisterScriptMethods);
}

// VERIFY: 2996b370 auto
// OFFSET: 0x00970800
void CSimpleColorSelect::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleColorSelect::s_metatable);
    CSimpleColorSelect::s_metatable = LUA_NOREF;
}
