#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 0fae3dcc auto
// OFFSET: 0x004A6020
void CSimpleAnim::CreateScriptMetaTable() {
    CSimpleAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleAnim::RegisterScriptMethods);
}

// VERIFY: ace702cb auto
// OFFSET: 0x004A4C50
void CSimpleAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleAnim::s_metatable);
    CSimpleAnim::s_metatable = LUA_NOREF;
}
