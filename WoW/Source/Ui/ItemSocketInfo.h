#ifndef WOW_SOURCE_UI_ITEMSOCKETINFO_H
#define WOW_SOURCE_UI_ITEMSOCKETINFO_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

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

class ItemStats_C;

class CGItemSocketInfo {
    public:
    static uint8_t unk_C20FA8[3];
    static WOWGUID m_gemItemContainer[3];
    static WOWGUID m_gemItems[3];
    static WOWGUID m_socketItem;
    static int32_t unk_C20FE8;

    static void EnterWorld();
    static int32_t GemMatchesSocket(const ItemStats_C* item, const ItemStats_C* gem, int32_t socket);
};

#endif
