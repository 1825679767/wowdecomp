#ifndef WOW_SOURCE_UI_ITEMSOCKETINFO_H
#define WOW_SOURCE_UI_ITEMSOCKETINFO_H

#include <cstdint>

struct lua_State;

int32_t Script_CloseSocketInfo(lua_State* L);
int32_t Script_GetSocketItemInfo(lua_State* L);
int32_t Script_GetNumSockets(lua_State* L);
int32_t Script_GetExistingSocketInfo(lua_State* L);
int32_t Script_GetExistingSocketLink(lua_State* L);
int32_t Script_GetNewSocketInfo(lua_State* L);
int32_t Script_GetNewSocketLink(lua_State* L);
int32_t Script_ClickSocketButton(lua_State* L);
int32_t Script_AcceptSockets(lua_State* L);
int32_t Script_GetSocketTypes(lua_State* L);
int32_t Script_GetSocketItemRefundable(lua_State* L);
int32_t Script_GetSocketItemBoundTradeable(lua_State* L);
void ItemSocketInfoRegisterScriptFunctions();

class CGItemSocketInfo {
    public:
    static void EnterWorld();
};

#endif
