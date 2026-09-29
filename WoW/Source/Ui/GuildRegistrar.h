#ifndef WOW_SOURCE_UI_GUILDREGISTRAR_H
#define WOW_SOURCE_UI_GUILDREGISTRAR_H

#include <cstdint>

struct lua_State;

int32_t Script_CloseGuildRegistrar(lua_State* L);
int32_t Script_GetGuildCharterCost(lua_State* L);
int32_t Script_BuyGuildCharter(lua_State* L);
int32_t Script_TurnInGuildCharter(lua_State* L);
int32_t Script_GetTabardInfo(lua_State* L);
void GuildRegistrarRegisterScriptFunctions();

class CGGuildRegistrar {
    public:
    static void EnterWorld();
};

#endif
