#ifndef WOW_SOURCE_UI_GUILDREGISTRAR_H
#define WOW_SOURCE_UI_GUILDREGISTRAR_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

int32_t Script_CloseGuildRegistrar(lua_State* L);
int32_t Script_BuyGuildCharter(lua_State* L);
int32_t Script_TurnInGuildCharter(lua_State* L);
int32_t Script_GetTabardInfo(lua_State* L);
void GuildRegistrarRegisterScriptFunctions();

struct GuildPetitionInfo {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t m_itemID;
    /* 0008 */ uint32_t m_displayID;
    /* 000C */ uint32_t m_cost;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
};

class CGGuildRegistrar {
    public:
    static WOWGUID m_registrar;
    static void CloseRegistrar();
    static void EnterWorld();
    static GuildPetitionInfo m_petition;
};

#endif
