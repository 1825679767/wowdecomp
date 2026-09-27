#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 5b9c53b9 auto
// OFFSET: 0x004A61D0
void CSimpleTranslationAnim::CreateScriptMetaTable() {
    CSimpleTranslationAnim::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleTranslationAnim::RegisterScriptMethods);
}

// VERIFY: 966e5a5a auto
// OFFSET: 0x004A4CB0
void CSimpleTranslationAnim::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleTranslationAnim::s_metatable);
    CSimpleTranslationAnim::s_metatable = LUA_NOREF;
}
