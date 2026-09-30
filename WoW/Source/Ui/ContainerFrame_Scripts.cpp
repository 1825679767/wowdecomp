#include "ContainerFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CGBankInfo.h"

// VERIFY: ebcdce34 auto
// OFFSET: 0x005D6FF0
static int32_t Script_GetMaxArenaCurrency(lua_State* L) {
    lua_pushnumber(L, 10000.0);
    return 1;
}

// VERIFY: fb1b77a0 manual
// OFFSET: 0x005D6F60
static int32_t Script_ContainerIDToInventoryID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ContainerIDToInventoryID(containerID)");
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (id > 10) {
        luaL_error(L, "ContainerIDToInventoryID(): invalid container ID");
        return 0;
    }

    int32_t slot = static_cast<int32_t>(id) >= 4 ? id + 64 : id + 20;
    lua_pushnumber(L, slot);
    return 1;
}

// VERIFY: d68e83af manual
// OFFSET: 0x005D7590
static int32_t Script_GetContainerNumFreeSlots(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetContainerFreeSlots(index)");
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    double index = lua_tonumber(L, 1);

    if (index == 0.0 || (index == -1.0 && CGBankInfo::m_banker)) {
        uint32_t first = index == 0.0 ? 23 : 39;
        uint32_t last = index == 0.0 ? 38 : 66;
        int32_t free = 0;

        for (uint32_t slot = first; slot <= last; slot++) {
            if (slot >= static_cast<uint32_t>(player->m_bag.m_numSlots) || !player->m_bag.m_slots[slot]) {
                free++;
            }
        }

        lua_pushnumber(L, free);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    if (index != -1.0 && static_cast<uint32_t>(static_cast<int32_t>(index) - 1) <= 10) {
        CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(GetBagAtIndex(static_cast<int32_t>(index) - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__));

        if (container) {
            lua_pushnumber(L, container->GetFreeSlots());
            lua_pushnumber(L, container->GetBagFamily());
            return 2;
        }
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: 68082673 manual
// OFFSET: 0x005D74A0
static int32_t Script_GetContainerNumSlots(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetContainerNumSlots(index)");
    }

    double slots = 0.0;

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (index == -1) {
            slots = 16.0;
        } else if (index == -2) {
            slots = 28.0;
        } else if (index == -3 || index == -5) {
            slots = 32.0;
        } else if (static_cast<uint32_t>(index) <= 10) {
            CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(GetBagAtIndex(index), TYPEMASK_CONTAINER, __FILE__, __LINE__));

            if (container) {
                slots = container->GetBag()->m_numSlots;
            }
        }
    }

    lua_pushnumber(L, slots);
    return 1;
}

// VERIFY: 27e4a857 manual
// OFFSET: 0x005D8C70
static int32_t Script_GetBagName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBagName(index)");
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        double index = lua_tonumber(L, 1);

        if (index == 0.0) {
            lua_pushstring(L, FrameScript_GetText("BACKPACK_TOOLTIP", -1, GENDER_NOT_APPLICABLE));
            return 1;
        }

        if (static_cast<uint32_t>(static_cast<int32_t>(index) - 1) <= 10) {
            CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(GetBagAtIndex(static_cast<int32_t>(index) - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__));

            if (container) {
                ItemStats_C* stats = g_itemDBCache.GetRecord(container->m_obj->m_entryID, WOWGUID(), nullptr, nullptr, false);

                if (stats) {
                    lua_pushstring(L, stats->GetName(0));
                    return 1;
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 9c9fb70c manual
// OFFSET: 0x005D7A90
static int32_t Script_GetContainerItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetContainerItemInfo(index, slot)");
    }

    CGBag_C* bag;
    int32_t slot;
    int32_t bank;

    if (!CGContainerInfo::GetLuaBagAndSlot(L, &bag, &slot, &bank)) {
        return 0;
    }

    CGItem_C* item = bag->GetItemPointer(slot);

    if (!item) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    lua_pushstring(L, texture);
    lua_pushnumber(L, item->m_item->m_itemFieldStackCount);

    if (item->m_itemFlags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, WOWGUID(), nullptr, nullptr, false);

    if (stats && stats->m_inventoryType) {
        lua_pushnumber(L, stats->m_quality);
    } else {
        lua_pushnumber(L, -1.0);
    }

    if (item->GetPageTextID(nullptr) || (item->m_item->m_itemFieldFlags & 0x200)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (item->GetItemStaticFlags(0) & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    return 7;
}

// STATIC: 0x00AD0D50
static FrameScript_Method s_ScriptFunctions[] = {
    { "ContainerIDToInventoryID", &Script_ContainerIDToInventoryID },
    { "GetContainerNumSlots", &Script_GetContainerNumSlots },
    { "GetContainerItemInfo", &Script_GetContainerItemInfo },
    { "GetContainerItemID", &Script_GetContainerItemID },
    { "GetContainerItemLink", &Script_GetContainerItemLink },
    { "GetContainerItemCooldown", &Script_GetContainerItemCooldown },
    { "PickupContainerItem", &Script_PickupContainerItem },
    { "SplitContainerItem", &Script_SplitContainerItem },
    { "UseContainerItem", &Script_UseContainerItem },
    { "SocketContainerItem", &Script_SocketContainerItem },
    { "ShowContainerSellCursor", &Script_ShowContainerSellCursor },
    { "SetBagPortraitTexture", &Script_SetBagPortraitTexture },
    { "GetBagName", &Script_GetBagName },
    { "GetContainerItemDurability", &Script_GetContainerItemDurability },
    { "GetContainerNumFreeSlots", &Script_GetContainerNumFreeSlots },
    { "GetContainerFreeSlots", &Script_GetContainerFreeSlots },
    { "GetContainerItemPurchaseInfo", &Script_GetContainerItemPurchaseInfo },
    { "GetContainerItemPurchaseItem", &Script_GetContainerItemPurchaseItem },
    { "ContainerRefundItemPurchase", &Script_ContainerRefundItemPurchase },
    { "GetMaxArenaCurrency", &Script_GetMaxArenaCurrency },
    { "GetContainerItemGems", &Script_GetContainerItemGems },
    { "GetContainerItemQuestInfo", &Script_GetContainerItemQuestInfo },
};

// VERIFY: 1248b250 auto
// OFFSET: 0x005D7020
void ContainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
