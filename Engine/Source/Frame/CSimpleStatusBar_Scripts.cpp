#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: bbf92432 auto
// OFFSET: 0x00971A20
void CSimpleStatusBar::CreateScriptMetaTable() {
    CSimpleStatusBar::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleStatusBar::RegisterScriptMethods);
}

// VERIFY: d47cea3b auto
// OFFSET: 0x009711D0
void CSimpleStatusBar::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleStatusBar::s_metatable);
    CSimpleStatusBar::s_metatable = LUA_NOREF;
}
