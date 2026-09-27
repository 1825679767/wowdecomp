#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 02dfb2f9 auto
// OFFSET: 0x004A6BF0
void CSimpleControlPoint::CreateScriptMetaTable() {
    CSimpleControlPoint::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleControlPoint::RegisterScriptMethods);
}

// VERIFY: 1011bc7c auto
// OFFSET: 0x004A4DC0
void CSimpleControlPoint::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleControlPoint::s_metatable);
    CSimpleControlPoint::s_metatable = LUA_NOREF;
}
