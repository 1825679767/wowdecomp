#ifndef WOW_SOURCE_UI_MERCHANTFRAME_H
#define WOW_SOURCE_UI_MERCHANTFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGMerchantInfo {
    public:
    static void EnterWorld();
    static WOWGUID m_merchant;
};

int32_t Script_CloseMerchant(lua_State* L);
int32_t Script_GetMerchantNumItems(lua_State* L);
int32_t Script_GetMerchantItemInfo(lua_State* L);
int32_t Script_GetMerchantItemCostInfo(lua_State* L);
int32_t Script_GetMerchantItemCostItem(lua_State* L);
int32_t Script_GetBuybackItemInfo(lua_State* L);
int32_t Script_GetBuybackItemLink(lua_State* L);
int32_t Script_GetMerchantItemLink(lua_State* L);
int32_t Script_GetMerchantItemMaxStack(lua_State* L);
int32_t Script_PickupMerchantItem(lua_State* L);
int32_t Script_BuyMerchantItem(lua_State* L);
int32_t Script_BuybackItem(lua_State* L);
int32_t Script_CanMerchantRepair(lua_State* L);
int32_t Script_ShowMerchantSellCursor(lua_State* L);
int32_t Script_ShowBuybackSellCursor(lua_State* L);
int32_t Script_ShowRepairCursor(lua_State* L);
int32_t Script_HideRepairCursor(lua_State* L);
int32_t Script_InRepairMode(lua_State* L);
int32_t Script_GetRepairAllCost(lua_State* L);
int32_t Script_RepairAllItems(lua_State* L);
int32_t Script_GetNumBuybackItems(lua_State* L);
void MerchantRegisterScriptFunctions();

#endif
