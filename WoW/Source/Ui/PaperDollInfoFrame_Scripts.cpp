#include "PaperDollInfoFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"

// VERIFY: c886e670 auto
// OFFSET: 0x005E9BC0
static int32_t Script_GetInventoryItemTexture(lua_State* L) {
    const char* token;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, token, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(GetUnitFromName(token));

    if (CGCharacterInfo::CanInspect(unit)) {
        int32_t displayId = 0;

        if (slot == -1) {
            if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
                int32_t ammoId = CGCharacterInfo::GetActivePlayerAmmoID(unit);

                if (ammoId && unit->GetBag()->GetItemTypeCount(ammoId, 0)) {
                    WOWGUID requester = unit->m_obj->m_guid;
                    ItemStats_C* stats = g_itemDBCache.GetRecord(ammoId, requester, UnitInventoryChangedCallback, nullptr, false);

                    if (stats) {
                        displayId = stats->m_displayId;
                    }
                }
            }
        } else {
            const VisibleItem_C* visible = unit->GetVisibleItem(slot);

            if (visible && visible->m_itemId) {
                displayId = const_cast<VisibleItem_C*>(visible)->GetDisplayID();
            } else {
                const CGItem_C* item = static_cast<const CGBag_C*>(unit->GetBag())->GetItemPointer(slot);

                if (item && !(item->m_modelFlags & 0x10000)) {
                    displayId = item->GetDisplayID();
                }
            }
        }

        if (displayId) {
            char texture[260];
            const char* path = ClientDBStringLookup(3);
            SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
            lua_pushstring(L, texture);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: cfe5fc3d auto
// OFFSET: 0x005EAC00
static int32_t Script_UpdateInventoryAlertStatus(lua_State* L) {
    CGCharacterInfo::UpdateInventoryAlerts();
    return 0;
}

// VERIFY: 3d87fc58 auto
// OFFSET: 0x005E7780
static int32_t Script_HasInspectHonorData(lua_State* L) {
    if (CGCharacterInfo::m_hasHonorData) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 6245d206 auto
// OFFSET: 0x005E7700
static int32_t Script_KeyRingButtonIDToInvSlotID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: KeyRingButtonIDToInvSlotID(buttonID)");
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(lua_tonumber(L, 1)) + 86);
    return 1;
}

// VERIFY: eec95ec5 manual
// OFFSET: 0x005EAC10
static int32_t Script_OffhandHasWeapon(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGItem_C* item = player ? player->m_bag.GetItemPointer(16) : nullptr;

    if (item && item->GetClassID() == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 23078a8f manual
// OFFSET: 0x005E7FA0
static int32_t Script_GetInventoryAlertStatus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInventoryAlertStatus(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t status = index >= CGCharacterInfo::m_invAlertStatus.m_count ? 0 : CGCharacterInfo::m_invAlertStatus.m_data[index];
    lua_pushnumber(L, status);
    return 1;
}

// VERIFY: cd7a1415 manual
// OFFSET: 0x005EAE90
static int32_t Script_HasWandEquipped(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<uint32_t>(player->m_bag.m_numSlots) > 17 && player->m_bag.m_slots[17]) {
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(player->m_bag.m_slots[17], TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && item->GetSubtypeID() == 19) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: ee365ef7 manual
// OFFSET: 0x005E77C0
static int32_t Script_GetInspectHonorData(lua_State* L) {
    lua_pushnumber(L, CGCharacterInfo::m_inspectTodayHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectTodayHonor);
    lua_pushnumber(L, CGCharacterInfo::m_inspectYesterdayHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectYesterdayHonor);
    lua_pushnumber(L, CGCharacterInfo::m_inspectLifetimeHonorableKills);
    lua_pushnumber(L, CGCharacterInfo::m_inspectLifetimeHighestRank);
    return 6;
}

// VERIFY: 10a2b29c manual
// OFFSET: 0x005E7D60
static int32_t Script_GetInventorySlotInfo(lua_State* L) {
    if (lua_isstring(L, 1)) {
        const char* name = lua_tolstring(L, 1, nullptr);
        int32_t count = g_paperDollItemFrameDB.m_numRecords;

        for (int32_t i = 0; i < count; i++) {
            PaperDollItemFrameRec* rec = i < 0 || i >= g_paperDollItemFrameDB.m_numRecords ? nullptr : &g_paperDollItemFrameDB.m_records[i];

            if (rec && !SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                lua_pushnumber(L, rec->m_slotNumber);
                lua_pushstring(L, rec->m_slotIcon);

                if (rec->m_slotNumber == 18) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                return 3;
            }
        }
    }

    luaL_error(L, "Invalid inventory slot in GetInventorySlotInfo");
    return 0;
}

// VERIFY: 3c72664e manual
// OFFSET: 0x005EA6E0
static int32_t Script_IsInventoryItemLocked(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        return 0;
    }

    CGPlayer_C* player = slot != -1 ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) : nullptr;
    const CGItem_C* item = player ? static_cast<const CGBag_C*>(&player->m_bag)->GetItemPointer(slot) : nullptr;

    if (item && (item->m_itemFlags & 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2fff9517 manual
// OFFSET: 0x005E7E60
static int32_t Script_GetInventoryItemCooldown(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit;
    CGItem_C* item;

    if (slot != -1 && CGCharacterInfo::CanInspect(unit = static_cast<CGPlayer_C*>(GetUnitFromName(unitName))) && (item = unit->m_bag.GetItemPointer(slot)) && item->GetEnchantmentSpell(0) && (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges())) {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        lua_pushnumber(L, start * 0.001);
        lua_pushnumber(L, duration * 0.001);
        lua_pushnumber(L, enable);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// VERIFY: fa692082 manual
// OFFSET: 0x005E9D80
static int32_t Script_GetInventoryItemBroken(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(GetUnitFromName(unitName));
    CGItem_C* item;

    if (!CGCharacterInfo::CanInspect(unit) || slot == -1 || !(item = unit->m_bag.GetItemPointer(slot)) || (item->m_item->m_itemFieldFlags & 0x8) || !item->m_item->m_itemFieldMaxdurability || item->GetDurability()) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: 7734206d manual
// OFFSET: 0x005E9E40
static int32_t Script_GetInventoryItemCount(lua_State* L) {
    const char* unitName;
    int32_t slot = 0;

    if (!GetUnitAndSlotFromLua(L, unitName, slot)) {
        return 0;
    }

    CGPlayer_C* unit = static_cast<CGPlayer_C*>(GetUnitFromName(unitName));

    if (!CGCharacterInfo::CanInspect(unit)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (slot == -1) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            int32_t ammo = CGCharacterInfo::GetActivePlayerAmmoID(unit);

            if (ammo) {
                lua_pushnumber(L, unit->GetBag()->GetItemTypeCount(ammo, 0));
                return 1;
            }
        }

        lua_pushnumber(L, 1.0);
        return 1;
    }

    CGItem_C* item = unit->m_bag.GetItemPointer(slot);

    if (!item) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (!(item->m_obj->m_type & TYPEMASK_CONTAINER)) {
        lua_pushnumber(L, item->m_item->m_itemFieldStackCount);
        return 1;
    }

    if (slot > 22) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID != static_cast<int32_t>(item->GetClassID()) || rec->m_subClassID != static_cast<int32_t>(item->GetSubtypeID())) {
            continue;
        }

        if (!(rec->m_displayFlags & 0x4)) {
            break;
        }

        if (item->GetBag()->GetItemTypeCount(-1, 0) <= 0) {
            lua_pushnumber(L, 0.0);
        } else {
            lua_pushnumber(L, item->GetBag()->GetItemTypeCount(-1, 0));
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: 864b2e9b auto
// OFFSET: 0x005EA5F0
static int32_t Script_PickupInventoryItem(lua_State* L) {
    int32_t slot = 0;

    if (ResolveInventorySlotArg(L, 1, slot)) {
        CGCharacterInfo::PickupInventoryItem(slot);
    }

    return 0;
}

// VERIFY: c662896d auto
// OFFSET: 0x005EA7F0
static int32_t Script_PutItemInBackpack(lua_State* L) {
    if (CGCharacterInfo::PutItemInBackpack(0xFF)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 307bdc3f manual
// OFFSET: 0x005EA880
static int32_t Script_CursorCanGoInSlot(lua_State* L) {
    int32_t slot = 0;

    if (!ResolveInventorySlotArg(L, 1, slot)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((player && player->ValidateSlot(slot, CGGameUI::GetCursorItem())) || (slot == -1 && CGGameUI::m_cursorItemType == 9)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: f4825e25 manual
// OFFSET: 0x005EACA0
static int32_t Script_GetWeaponEnchantInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 6;
    }

    for (int32_t slot = 15; slot <= 16; slot++) {
        CGItem_C* item = player->m_bag.GetItemPointer(slot);

        if (item && item->GetEnchantmentId(1)) {
            lua_pushnumber(L, 1.0);

            if (item->GetEnchantmentDuration(1)) {
                lua_pushnumber(L, item->GetEnchantmentTimeLeft(1));
            } else {
                lua_pushnil(L);
            }

            lua_pushnumber(L, item->GetEnchantmentCharges(1));
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    return 6;
}

// STATIC: 0x00AD1270
// VERIFY: d5cfb323 manual
// OFFSET: 0x005EAAF0
static int32_t Script_GetGuildInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetGuildInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);
        int32_t guildId = player->m_player->m_playerGuildid;

        if (guildId) {
            WOWGUID guid = unit->m_obj->m_guid;
            GuildStats_C* guild = g_guildInfoCache.GetRecord(guildId, guid, GuildNameCallback, nullptr, false);

            if (guild) {
                char rankName[64];
                SStrCopy(rankName, guild->m_rankNames[player->m_player->m_playerGuildrank], STORM_MAX_STR);
                lua_pushstring(L, guild->m_name);
                lua_pushstring(L, rankName);
                lua_pushnumber(L, player->m_player->m_playerGuildrank);
                return 3;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    return 3;
}

static FrameScript_Method s_ScriptFunctions[] = {
    { "GetInventorySlotInfo", &Script_GetInventorySlotInfo },
    { "GetInventoryItemsForSlot", &Script_GetInventoryItemsForSlot },
    { "GetInventoryItemTexture", &Script_GetInventoryItemTexture },
    { "GetInventoryItemBroken", &Script_GetInventoryItemBroken },
    { "GetInventoryItemCount", &Script_GetInventoryItemCount },
    { "GetInventoryItemQuality", &Script_GetInventoryItemQuality },
    { "GetInventoryItemCooldown", &Script_GetInventoryItemCooldown },
    { "GetInventoryItemDurability", &Script_GetInventoryItemDurability },
    { "GetInventoryItemLink", &Script_GetInventoryItemLink },
    { "GetInventoryItemID", &Script_GetInventoryItemID },
    { "GetInventoryItemGems", &Script_GetInventoryItemGems },
    { "KeyRingButtonIDToInvSlotID", &Script_KeyRingButtonIDToInvSlotID },
    { "PickupInventoryItem", &Script_PickupInventoryItem },
    { "UseInventoryItem", &Script_UseInventoryItem },
    { "SocketInventoryItem", &Script_SocketInventoryItem },
    { "IsInventoryItemLocked", &Script_IsInventoryItemLocked },
    { "PutItemInBag", &Script_PutItemInBag },
    { "PutItemInBackpack", &Script_PutItemInBackpack },
    { "PickupBagFromSlot", &Script_PickupBagFromSlot },
    { "CursorCanGoInSlot", &Script_CursorCanGoInSlot },
    { "ShowInventorySellCursor", &Script_ShowInventorySellCursor },
    { "SetInventoryPortraitTexture", &Script_SetInventoryPortraitTexture },
    { "GetGuildInfo", &Script_GetGuildInfo },
    { "GetInventoryAlertStatus", &Script_GetInventoryAlertStatus },
    { "UpdateInventoryAlertStatus", &Script_UpdateInventoryAlertStatus },
    { "OffhandHasWeapon", &Script_OffhandHasWeapon },
    { "HasInspectHonorData", &Script_HasInspectHonorData },
    { "RequestInspectHonorData", &Script_RequestInspectHonorData },
    { "GetInspectHonorData", &Script_GetInspectHonorData },
    { "GetInspectArenaTeamData", &Script_GetInspectArenaTeamData },
    { "ClearInspectPlayer", &Script_ClearInspectPlayer },
    { "GetWeaponEnchantInfo", &Script_GetWeaponEnchantInfo },
    { "HasWandEquipped", &Script_HasWandEquipped },
};

// VERIFY: ee30d944 auto
// OFFSET: 0x005E78F0
void CharacterInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
