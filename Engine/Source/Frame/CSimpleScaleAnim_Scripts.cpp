#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 044ce984 auto
// OFFSET: 0x004A6770
void CSimpleScaleAnim::CreateScriptMetaTable() {
    CSimpleScaleAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleScaleAnim::RegisterScriptMethods);
}

// VERIFY: 1acf695f auto
// OFFSET: 0x004A4D70
void CSimpleScaleAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleScaleAnim::s_metatable);
    CSimpleScaleAnim::s_metatable = LUA_NOREF;
}
