#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: e54f54a8 auto
// OFFSET: 0x004A1E60
void CSimpleFrame::CreateScriptMetaTable() {
    CSimpleFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleFrame::RegisterScriptMethods);
}

// VERIFY: d0ab2718 auto
// OFFSET: 0x0049E560
void CSimpleFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleFrame::s_metatable);
    CSimpleFrame::s_metatable = LUA_NOREF;
}
