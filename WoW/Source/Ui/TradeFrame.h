#ifndef WOW_SOURCE_UI_TRADEFRAME_H
#define WOW_SOURCE_UI_TRADEFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGTradeInfo {
    public:
    static void EnterWorld();
    static WOWGUID m_tradingPlayer;

    static void UpdatePlayerItem(WOWGUID guid);
    static void AddCursorItem(int32_t bound);
    static void InitiateTrade(WOWGUID guid, int32_t a2);
};

int32_t Script_CloseTrade(lua_State* L);
int32_t Script_ClickTradeButton(lua_State* L);
int32_t Script_ClickTargetTradeButton(lua_State* L);
int32_t Script_GetTradeTargetItemInfo(lua_State* L);
int32_t Script_GetTradeTargetItemLink(lua_State* L);
int32_t Script_GetTradePlayerItemInfo(lua_State* L);
int32_t Script_GetTradePlayerItemLink(lua_State* L);
int32_t Script_AcceptTrade(lua_State* L);
int32_t Script_CancelTradeAccept(lua_State* L);
int32_t Script_GetPlayerTradeMoney(lua_State* L);
int32_t Script_GetTargetTradeMoney(lua_State* L);
int32_t Script_PickupTradeMoney(lua_State* L);
int32_t Script_AddTradeMoney(lua_State* L);
int32_t Script_SetTradeMoney(lua_State* L);
void TradeInfoRegisterScriptFunctions();

#endif
