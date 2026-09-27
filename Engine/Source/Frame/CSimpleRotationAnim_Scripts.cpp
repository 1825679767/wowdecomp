#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 3538788e auto
// OFFSET: 0x004A64E0
void CSimpleRotationAnim::CreateScriptMetaTable() {
    CSimpleRotationAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleRotationAnim::RegisterScriptMethods);
}

// VERIFY: ed18cc33 auto
// OFFSET: 0x004A4D10
void CSimpleRotationAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleRotationAnim::s_metatable);
    CSimpleRotationAnim::s_metatable = LUA_NOREF;
}
