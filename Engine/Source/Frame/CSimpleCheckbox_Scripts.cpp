#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 19366438 auto
// OFFSET: 0x00976FB0
void CSimpleCheckbox::CreateScriptMetaTable() {
    CSimpleCheckbox::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleCheckbox::RegisterScriptMethods);
}

// VERIFY: dd86ea1c auto
// OFFSET: 0x00976B10
void CSimpleCheckbox::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleCheckbox::s_metatable);
    CSimpleCheckbox::s_metatable = LUA_NOREF;
}
