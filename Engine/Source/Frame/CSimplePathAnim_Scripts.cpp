#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: fe85cabc auto
// OFFSET: 0x004A6D80
void CSimplePathAnim::CreateScriptMetaTable() {
    CSimplePathAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimplePathAnim::RegisterScriptMethods);
}

// VERIFY: edb69ae2 auto
// OFFSET: 0x004A4E20
void CSimplePathAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimplePathAnim::s_metatable);
    CSimplePathAnim::s_metatable = LUA_NOREF;
}
