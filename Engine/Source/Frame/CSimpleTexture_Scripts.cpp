#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 83e676f3 auto
// OFFSET: 0x0048CD00
void CSimpleTexture::CreateScriptMetaTable() {
    CSimpleTexture::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleTexture::RegisterScriptMethods);
}

// VERIFY: e6dffeb5 auto
// OFFSET: 0x0048BC40
void CSimpleTexture::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleTexture::s_metatable);
    CSimpleTexture::s_metatable = LUA_NOREF;
}
