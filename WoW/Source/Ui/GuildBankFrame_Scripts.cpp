#include "GuildBankFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GuildInfo.h"

// STATIC: 0x00A15EEC
static const uint32_t s_guildBankTabCosts[6] = { 1000000, 2500000, 5000000, 10000000, 25000000, 50000000 };

// VERIFY: 245690a9 auto
// OFFSET: 0x005A4410
static int32_t Script_GetCurrentGuildBankTab(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_currentTab + 1);
    return 1;
}

// VERIFY: d2e0f5e2 auto
// OFFSET: 0x005A44A0
static int32_t Script_GetGuildBankWithdrawMoney(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_remainingWithdrawMoney);
    return 1;
}

// VERIFY: 424f18ba auto
// OFFSET: 0x005A44D0
static int32_t Script_GetNumGuildBankTabs(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_numTabs);
    return 1;
}

// VERIFY: e24dcd33 auto
// OFFSET: 0x005A45B0
static int32_t Script_GetNumGuildBankMoneyTransactions(lua_State* L) {
    lua_pushnumber(L, CGGuildBankInfo::m_numMoneyTransactions);
    return 1;
}

// VERIFY: 1c924350 auto
// OFFSET: 0x005A43C0
static int32_t Script_SetCurrentGuildBankTab(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetCurrentTab(tab)");
        return 0;
    }

    uint32_t tab = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (tab < 6) {
        CGGuildBankInfo::m_currentTab = tab;
    }

    return 0;
}

// VERIFY: 4de92797 auto
// OFFSET: 0x005A4440
static int32_t Script_GetGuildBankMoney(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGuildBankInfo::m_money));
    return 1;
}

// VERIFY: faf0c262 manual
// OFFSET: 0x005A6B20
static int32_t Script_GetGuildBankTabCost(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t cost;

    if (CGGuildBankInfo::m_numTabs < 6 && (cost = s_guildBankTabCosts[CGGuildBankInfo::m_numTabs])) {
        lua_pushnumber(L, cost);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: be98e965 manual
// OFFSET: 0x005A5F60
static int32_t Script_CanWithdrawGuildBankMoney(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid) {
        const GuildRankInfo& rank = CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank];

        if (rank.m_bankMoneyPerDay && (rank.m_rights & 0x80000)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00ACF8D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "QueryGuildBankTab", &Script_QueryGuildBankTab },
    { "SetCurrentGuildBankTab", &Script_SetCurrentGuildBankTab },
    { "GetCurrentGuildBankTab", &Script_GetCurrentGuildBankTab },
    { "GetGuildBankItemInfo", &Script_GetGuildBankItemInfo },
    { "SetGuildBankTabInfo", &Script_SetGuildBankTabInfo },
    { "GetGuildBankItemLink", &Script_GetGuildBankItemLink },
    { "PickupGuildBankItem", &Script_PickupGuildBankItem },
    { "AutoStoreGuildBankItem", &Script_AutoStoreGuildBankItem },
    { "SplitGuildBankItem", &Script_SplitGuildBankItem },
    { "GetNumGuildBankTabs", &Script_GetNumGuildBankTabs },
    { "GetGuildBankTabInfo", &Script_GetGuildBankTabInfo },
    { "GetGuildBankTabCost", &Script_GetGuildBankTabCost },
    { "BuyGuildBankTab", &Script_BuyGuildBankTab },
    { "DepositGuildBankMoney", &Script_DepositGuildBankMoney },
    { "WithdrawGuildBankMoney", &Script_WithdrawGuildBankMoney },
    { "CanWithdrawGuildBankMoney", &Script_CanWithdrawGuildBankMoney },
    { "PickupGuildBankMoney", &Script_PickupGuildBankMoney },
    { "GetGuildBankMoney", &Script_GetGuildBankMoney },
    { "GetGuildBankWithdrawMoney", &Script_GetGuildBankWithdrawMoney },
    { "CloseGuildBankFrame", &Script_CloseGuildBankFrame },
    { "GetGuildTabardFileNames", &Script_GetGuildTabardFileNames },
    { "QueryGuildBankLog", &Script_QueryGuildBankLog },
    { "GetNumGuildBankTransactions", &Script_GetNumGuildBankTransactions },
    { "GetGuildBankTransaction", &Script_GetGuildBankTransaction },
    { "GetNumGuildBankMoneyTransactions", &Script_GetNumGuildBankMoneyTransactions },
    { "GetGuildBankMoneyTransaction", &Script_GetGuildBankMoneyTransaction },
    { "QueryGuildBankText", &Script_QueryGuildBankText },
    { "GetGuildBankText", &Script_GetGuildBankText },
    { "SetGuildBankText", &Script_SetGuildBankText },
};

// VERIFY: 91a3bc69 auto
// OFFSET: 0x005A46A0
void GuildBankRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
