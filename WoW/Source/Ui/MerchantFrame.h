#ifndef WOW_SOURCE_UI_MERCHANTFRAME_H
#define WOW_SOURCE_UI_MERCHANTFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct MerchantItem {
    /* 0000 */ int32_t m_slot;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_displayID;
    /* 000C */ int32_t m_maxCount;
    /* 0010 */ int32_t m_price;
    /* 0014 */ int32_t m_durability;
    /* 0018 */ int32_t m_buyCount;
    /* 001C */ int32_t m_extendedCostID;
};

class CGMerchantInfo {
    public:
    static void EnterWorld();
    static MerchantItem m_items[150];
    static WOWGUID m_merchant;
    static int32_t m_numMerchantItems;
    static uint32_t m_numBuybackItems;
};

int32_t Script_CloseMerchant(lua_State* L);
int32_t Script_GetMerchantItemInfo(lua_State* L);
int32_t Script_GetMerchantItemCostItem(lua_State* L);
int32_t Script_GetBuybackItemInfo(lua_State* L);
int32_t Script_GetBuybackItemLink(lua_State* L);
int32_t Script_GetMerchantItemLink(lua_State* L);
int32_t Script_PickupMerchantItem(lua_State* L);
int32_t Script_BuyMerchantItem(lua_State* L);
int32_t Script_BuybackItem(lua_State* L);
int32_t Script_ShowMerchantSellCursor(lua_State* L);
int32_t Script_ShowBuybackSellCursor(lua_State* L);
int32_t Script_ShowRepairCursor(lua_State* L);
int32_t Script_HideRepairCursor(lua_State* L);
int32_t Script_GetRepairAllCost(lua_State* L);
int32_t Script_RepairAllItems(lua_State* L);
void MerchantRegisterScriptFunctions();

#endif
