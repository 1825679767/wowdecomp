#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: b274e4b5 auto
// OFFSET: 0x009720C0
void CSimpleSlider::CreateScriptMetaTable() {
    CSimpleSlider::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleSlider::RegisterScriptMethods);
}

// VERIFY: e4dec925 auto
// OFFSET: 0x00971A60
void CSimpleSlider::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleSlider::s_metatable);
    CSimpleSlider::s_metatable = LUA_NOREF;
}
