#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 88a11534 auto
// OFFSET: 0x00978400
void CSimpleButton::CreateScriptMetaTable() {
    CSimpleButton::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleButton::RegisterScriptMethods);
}

// VERIFY: a9654e41 auto
// OFFSET: 0x00977060
void CSimpleButton::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleButton::s_metatable);
    CSimpleButton::s_metatable = LUA_NOREF;
}
