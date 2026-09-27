#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 7cc86a8e auto
// OFFSET: 0x00961540
void CSimpleModel::CreateScriptMetaTable() {
    CSimpleModel::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleModel::RegisterScriptMethods);
}

// VERIFY: 454a72eb auto
// OFFSET: 0x009603F0
void CSimpleModel::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleModel::s_metatable);
    CSimpleModel::s_metatable = LUA_NOREF;
}
