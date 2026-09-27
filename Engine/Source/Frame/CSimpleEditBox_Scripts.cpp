#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleEditBox.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 0ebf05e3 auto
// OFFSET: 0x00976AD0
void CSimpleEditBox::CreateScriptMetaTable() {
    CSimpleEditBox::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleEditBox::RegisterScriptMethods);
}

// VERIFY: 10d61e61 auto
// OFFSET: 0x009752E0
void CSimpleEditBox::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleEditBox::s_metatable);
    CSimpleEditBox::s_metatable = LUA_NOREF;
}
