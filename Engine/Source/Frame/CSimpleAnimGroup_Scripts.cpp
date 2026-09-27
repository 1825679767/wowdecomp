#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: d2ab9274 auto
// OFFSET: 0x004A79B0
void CSimpleAnimGroup::CreateScriptMetaTable() {
    CSimpleAnimGroup::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleAnimGroup::RegisterScriptMethods);
}

// VERIFY: 0807309e auto
// OFFSET: 0x004A4ED0
void CSimpleAnimGroup::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleAnimGroup::s_metatable);
    CSimpleAnimGroup::s_metatable = LUA_NOREF;
}
