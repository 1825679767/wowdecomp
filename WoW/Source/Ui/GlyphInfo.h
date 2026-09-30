#ifndef WOW_SOURCE_UI_GLYPHINFO_H
#define WOW_SOURCE_UI_GLYPHINFO_H

#include <cstdint>

struct lua_State;

int32_t Script_GetGlyphSocketInfo(lua_State* L);
int32_t Script_GlyphMatchesSocket(lua_State* L);
int32_t Script_PlaceGlyphInSocket(lua_State* L);
int32_t Script_GetGlyphLink(lua_State* L);
void GlyphInfoRegisterScriptFunctions();

#endif
