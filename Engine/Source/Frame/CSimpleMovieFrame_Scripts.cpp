#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// INLINED
void CSimpleMovieFrame::CreateScriptMetaTable() {
    CSimpleMovieFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(FrameScript_GetContext(), &CSimpleMovieFrame::RegisterScriptMethods);
}

// INLINED
void CSimpleMovieFrame::DestroyScriptMetaTable() {
    luaL_unref(FrameScript_GetContext(), LUA_REGISTRYINDEX, CSimpleMovieFrame::s_metatable);
    CSimpleMovieFrame::s_metatable = LUA_NOREF;
}
