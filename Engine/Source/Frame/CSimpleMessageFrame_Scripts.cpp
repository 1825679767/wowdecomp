#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMessageFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 792a676f auto
// OFFSET: 0x009748D0
void CSimpleMessageFrame::CreateScriptMetaTable() {
    CSimpleMessageFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleMessageFrame::RegisterScriptMethods);
}

// VERIFY: c87b4319 auto
// OFFSET: 0x00973D40
void CSimpleMessageFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleMessageFrame::s_metatable);
    CSimpleMessageFrame::s_metatable = LUA_NOREF;
}
