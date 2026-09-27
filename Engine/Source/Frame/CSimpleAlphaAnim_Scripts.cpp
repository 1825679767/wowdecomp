#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: cd7dd594 auto
// OFFSET: 0x004A6E80
void CSimpleAlphaAnim::CreateScriptMetaTable() {
    CSimpleAlphaAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleAlphaAnim::RegisterScriptMethods);
}

// VERIFY: 608d8f16 auto
// OFFSET: 0x004A4E80
void CSimpleAlphaAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleAlphaAnim::s_metatable);
    CSimpleAlphaAnim::s_metatable = LUA_NOREF;
}
