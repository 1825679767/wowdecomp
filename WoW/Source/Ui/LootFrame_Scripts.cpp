#include "LootFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/Object/ObjectClient/LootRoll.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/Types.h"

// VERIFY: 58fe7575 manual
// OFFSET: 0x00588CC0
static int32_t Script_GetLootRollTimeLeft(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootRollTimeLeft(id)");
    }

    PendingLootRoll* roll = FindRoll(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (roll) {
        lua_pushnumber(L, static_cast<int32_t>(roll->m_expireTime - OsGetAsyncTimeMs()));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: aafe1aca manual
// OFFSET: 0x00588750
static int32_t Script_LootSlotIsItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlotIsItem(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t index = slot - 1;

    if (CGLootInfo::m_lootObj) {
        if (CGLootInfo::m_coins) {
            if (slot == 1) {
                lua_pushnil(L);
                return 1;
            }

            index = slot - 2;
        }

        if (index < 18 && CGLootInfo::m_slots[index].m_itemID > 0) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: f8d54875 manual
// OFFSET: 0x00588810
static int32_t Script_LootSlotIsCoin(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: LootSlotIsCoin(slot)");
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));

    if (CGLootInfo::m_lootObj && CGLootInfo::m_coins > 0 && slot == 1) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 7c2b4fef auto
// OFFSET: 0x005888E0
static int32_t Script_IsFishingLoot(lua_State* L) {
    if (CGLootInfo::m_lootType == 3) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: a7a1243d manual
// OFFSET: 0x005889D0
static int32_t Script_GetLootRollItemInfo(lua_State* L) {
    char buffer[256];

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLootRollItemInfo(id)");
        return 0;
    }

    PendingLootRoll* roll = FindRoll(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!roll) {
        return 0;
    }

    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(roll->m_itemId, none, nullptr, nullptr, false);

    if (!stats) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);
    const char* separator = *path ? "\\" : "";
    SStrPrintf(buffer, sizeof(buffer), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(stats->m_displayId));
    lua_pushstring(L, buffer);
    CGItem_C::BuildItemName(buffer, sizeof(buffer), roll->m_itemId, roll->m_randomPropertyId);
    lua_pushstring(L, buffer);
    lua_pushnumber(L, roll->m_count);
    lua_pushnumber(L, static_cast<int32_t>(stats->m_quality));

    if (stats->m_bonding == 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roll->m_voteMask & 8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, roll->m_reasonNeed);
    lua_pushnumber(L, roll->m_reasonGreed);
    lua_pushnumber(L, roll->m_reasonDisenchant);
    lua_pushnumber(L, static_cast<int32_t>(stats->m_requiredDisenchantSkill));
    return 12;
}

// VERIFY: a8f76425 auto
// OFFSET: 0x00588150
static void MasterLootCandidateNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_MASTER_LOOT_LIST, nullptr);
    }
}

// VERIFY: 7818e364 manual
// OFFSET: 0x00588920
static int32_t Script_GetMasterLootCandidate(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMasterLootCandidate(index)");
    }

    WOWGUID guid = GetMasterLootCandidate(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1)) - 1));
    NameCache* record = guid != WOWGUID() ? g_nameDBCache.GetRecord(guid.m_guid, guid, MasterLootCandidateNameCallback, nullptr, false) : nullptr;

    if (record) {
        lua_pushstring(L, record->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00ACEE28
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLootPortrait", &Script_SetLootPortrait },
    { "GetNumLootItems", &Script_GetNumLootItems },
    { "GetLootSlotInfo", &Script_GetLootSlotInfo },
    { "GetLootSlotLink", &Script_GetLootSlotLink },
    { "LootSlotIsItem", &Script_LootSlotIsItem },
    { "LootSlotIsCoin", &Script_LootSlotIsCoin },
    { "LootSlot", &Script_LootSlot },
    { "ConfirmLootSlot", &Script_ConfirmLootSlot },
    { "CloseLoot", &Script_CloseLoot },
    { "IsFishingLoot", &Script_IsFishingLoot },
    { "GetMasterLootCandidate", &Script_GetMasterLootCandidate },
    { "GiveMasterLoot", &Script_GiveMasterLoot },
    { "GetLootRollItemInfo", &Script_GetLootRollItemInfo },
    { "GetLootRollItemLink", &Script_GetLootRollItemLink },
    { "GetLootRollTimeLeft", &Script_GetLootRollTimeLeft },
    { "RollOnLoot", &Script_RollOnLoot },
    { "ConfirmLootRoll", &Script_ConfirmLootRoll },
};

// VERIFY: 25122094 auto
// OFFSET: 0x00588E60
void LootInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
