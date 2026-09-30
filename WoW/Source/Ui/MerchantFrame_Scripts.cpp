#include "MerchantFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"
#include "WoW/Source/Object/ItemStats.h"

// VERIFY: 32f102ca auto
// OFFSET: 0x005841D0
static int32_t Script_GetMerchantNumItems(lua_State* L) {
    lua_pushnumber(L, CGMerchantInfo::m_numMerchantItems);
    return 1;
}

// VERIFY: c2f7524a manual
// OFFSET: 0x005843F0
static int32_t Script_GetNumBuybackItems(lua_State* L) {
    if (CGMerchantInfo::m_merchant) {
        lua_pushnumber(L, CGMerchantInfo::m_numBuybackItems);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: c986aee1 manual
// OFFSET: 0x005849F0
static int32_t Script_CanMerchantRepair(lua_State* L) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGMerchantInfo::m_merchant, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && (unit->m_unit->m_unitNpcFlags & 0x1000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 9422b5ea auto
// OFFSET: 0x005843B0
static int32_t Script_InRepairMode(lua_State* L) {
    if (CursorGetResetMode() == 17) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2137ed3a manual
// OFFSET: 0x005842D0
static int32_t Script_GetMerchantItemMaxStack(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemMaxStack(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MerchantItem* item = index >= 0 && index < CGMerchantInfo::m_numMerchantItems && CGMerchantInfo::m_merchant ? &CGMerchantInfo::m_items[index] : nullptr;
    ItemStats_C* stats;
    double maxStack;

    if (item && item->m_buyCount <= 1 && item->m_itemID && (stats = g_itemDBCache.GetRecord(item->m_itemID, WOWGUID(), nullptr, nullptr, false))) {
        maxStack = stats->m_stackable;
    } else {
        maxStack = 1.0;
    }

    lua_pushnumber(L, maxStack);
    return 1;
}

// VERIFY: b19e5ef0 auto
// OFFSET: 0x005846E0
static int32_t Script_GetMerchantItemCostInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMerchantItemCostInfo(index)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MerchantItem* item = index >= 0 && index < CGMerchantInfo::m_numMerchantItems && CGMerchantInfo::m_merchant ? &CGMerchantInfo::m_items[index] : nullptr;
    ItemExtendedCostRec* cost;
    double numItems;

    if (item && item->m_itemID && (cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID))) {
        lua_pushnumber(L, cost->m_honorPoints);
        lua_pushnumber(L, cost->m_arenaPoints);
        int32_t count = cost->m_itemID[0] != 0;

        if (cost->m_itemID[1]) {
            count++;
        }

        if (cost->m_itemID[2]) {
            count++;
        }

        if (cost->m_itemID[3]) {
            count++;
        }

        if (cost->m_itemID[4]) {
            count++;
        }

        numItems = count;
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        numItems = 0.0;
    }

    lua_pushnumber(L, numItems);
    return 3;
}

// STATIC: 0x00ACED00
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseMerchant", &Script_CloseMerchant },
    { "GetMerchantNumItems", &Script_GetMerchantNumItems },
    { "GetMerchantItemInfo", &Script_GetMerchantItemInfo },
    { "GetMerchantItemCostInfo", &Script_GetMerchantItemCostInfo },
    { "GetMerchantItemCostItem", &Script_GetMerchantItemCostItem },
    { "GetBuybackItemInfo", &Script_GetBuybackItemInfo },
    { "GetBuybackItemLink", &Script_GetBuybackItemLink },
    { "GetMerchantItemLink", &Script_GetMerchantItemLink },
    { "GetMerchantItemMaxStack", &Script_GetMerchantItemMaxStack },
    { "PickupMerchantItem", &Script_PickupMerchantItem },
    { "BuyMerchantItem", &Script_BuyMerchantItem },
    { "BuybackItem", &Script_BuybackItem },
    { "CanMerchantRepair", &Script_CanMerchantRepair },
    { "ShowMerchantSellCursor", &Script_ShowMerchantSellCursor },
    { "ShowBuybackSellCursor", &Script_ShowBuybackSellCursor },
    { "ShowRepairCursor", &Script_ShowRepairCursor },
    { "HideRepairCursor", &Script_HideRepairCursor },
    { "InRepairMode", &Script_InRepairMode },
    { "GetRepairAllCost", &Script_GetRepairAllCost },
    { "RepairAllItems", &Script_RepairAllItems },
    { "GetNumBuybackItems", &Script_GetNumBuybackItems },
};

// VERIFY: 47ef76cb auto
// OFFSET: 0x00584450
void MerchantRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
