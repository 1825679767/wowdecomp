#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: cb237dcc auto
// OFFSET: 0x0048E150
void CSimpleFontString::CreateScriptMetaTable() {
    CSimpleFontString::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleFontString::RegisterScriptMethods);
}

// VERIFY: ac50861d auto
// OFFSET: 0x0048BCC0
void CSimpleFontString::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleFontString::s_metatable);
    CSimpleFontString::s_metatable = LUA_NOREF;
}
