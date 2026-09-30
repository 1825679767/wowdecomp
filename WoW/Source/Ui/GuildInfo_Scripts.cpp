#include "GuildInfo.h"
#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"


// STATIC: 0x00AD07B8
static GuildRankFlag s_guildRankFlags[17] = {
    { 0x1, 0 },
    { 0x2, 0 },
    { 0x4, 0 },
    { 0x8, 0 },
    { 0x80, 0 },
    { 0x100, 0 },
    { 0x10, 0 },
    { 0x20, 0 },
    { 0x1000, 0 },
    { 0x2000, 0 },
    { 0x4000, 0 },
    { 0x8000, 0 },
    { 0x10000, 0 },
    { 0x20000, 0 },
    { 0x40000, 0 },
    { 0x80000, 0 },
    { 0x100000, 0 },
};

// STATIC: 0x00AD07AC
static uint32_t s_guildBankTabPermissionFlags[3] = { 1, 2, 4 };

// VERIFY: d783a8ca auto
// OFFSET: 0x005CA1B0
static int32_t Script_SetGuildRosterShowOffline(lua_State* L) {
    g_guildShowOfflineCVar->Set(StringToBOOL(L, 1, 1), true, false, false, true);
    return 0;
}

// VERIFY: d633c72c auto
// OFFSET: 0x005CA350
static int32_t Script_GetNumGuildEvents(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_numEventLogEntries);
    return 1;
}

// VERIFY: ca7226df auto
// OFFSET: 0x005CA1E0
static int32_t Script_GetGuildRosterShowOffline(lua_State* L) {
    if (g_guildShowOfflineCVar->m_intValue) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 84ec1a39 manual
// OFFSET: 0x005CA220
static int32_t Script_GuildControlGetNumRanks(lua_State* L) {
    lua_pushnumber(L, CGGuildInfo::m_numRanks);
    return 1;
}

// VERIFY: 8bf71aee manual
// OFFSET: 0x005CA2B0
static int32_t Script_GuildControlSetRankFlag(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GuildControlSetRankFlag(index, enabled)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 17) {
        s_guildRankFlags[index].m_enabled = StringToBOOL(L, 2, 0);
    }

    return 0;
}

// VERIFY: 45ed1e52 manual
// OFFSET: 0x005CB3F0
static int32_t Script_GuildControlDelRank(lua_State* L) {
    if (CGGuildInfo::m_numRanks > 5) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_GUILD_DELETE_RANK));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// VERIFY: ca544327 manual
// OFFSET: 0x005CC360
static int32_t Script_CanEditGuildEvent(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x100000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 468bb570 manual
// OFFSET: 0x005CBCF0
static int32_t Script_CanGuildPromote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x80)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 29e69846 manual
// OFFSET: 0x005CBD80
static int32_t Script_CanGuildDemote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x100)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 383050bb manual
// OFFSET: 0x005CBE10
static int32_t Script_CanGuildInvite(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x10)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 27f3ef3a manual
// OFFSET: 0x005CBEA0
static int32_t Script_CanGuildRemove(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x20)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 41636616 manual
// OFFSET: 0x005CBF30
static int32_t Script_CanEditMOTD(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x1000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: d8bb8cb1 manual
// OFFSET: 0x005CBFC0
static int32_t Script_CanEditPublicNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x2000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 43a3c0dd manual
// OFFSET: 0x005CC050
static int32_t Script_CanEditOfficerNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x8000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: dee3d37b manual
// OFFSET: 0x005CC0E0
static int32_t Script_CanViewOfficerNote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x4000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 1491f340 manual
// OFFSET: 0x005CC170
static int32_t Script_CanEditGuildInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x10000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: d13b103b manual
// OFFSET: 0x005CC200
static int32_t Script_CanGuildBankRepair(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && (CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_rights & 0x40000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9a060c1e manual
// OFFSET: 0x005CC3E0
static int32_t Script_GuildControlGetRankFlags(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t rights = CGGuildInfo::m_rankInfo[s_guildControlRank].m_rights;

    for (uint32_t i = 0; i < 17; i++) {
        if (player && player->m_player->m_playerGuildid && (rights & s_guildRankFlags[i].m_mask)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    }

    return 17;
}

// VERIFY: ed348c21 auto
// OFFSET: 0x005CB6A0
static int32_t Script_SetGuildBankWithdrawLimit(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_SetGuildBankWithdrawLimit(amount)");
    }

    uint32_t amount = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (amount > 1000000000) {
        amount = 1000000000;
    }

    CGGuildInfo::m_rankInfo[s_guildControlRank].m_bankMoneyPerDay = amount;
    return 0;
}

// VERIFY: c31fa032 manual
// OFFSET: 0x005CB700
static int32_t Script_GetGuildBankWithdrawLimit(lua_State* L) {
    int32_t limit = CGGuildInfo::m_rankInfo[s_guildControlRank].m_bankMoneyPerDay;

    if (limit >= 0) {
        limit /= 10000;
    }

    lua_pushnumber(L, limit);
    return 1;
}

// VERIFY: 7fbb572a manual
// OFFSET: 0x005CB760
static int32_t Script_SetGuildBankTabWithdraw(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildBankTabWithdraw(tab, amount)");
    }

    uint32_t tab = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t amount = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (amount > 100000) {
        amount = 100000;
    }

    CGGuildInfo::m_rankInfo[s_guildControlRank].m_bankTabStacksPerDay[tab] = amount;
    return 0;
}

// VERIFY: 7598cb74 manual
// OFFSET: 0x005CC470
static int32_t Script_QueryGuildEventLog(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_GUILD_EVENT_LOG_QUERY));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// VERIFY: 9a95bfcb manual
// OFFSET: 0x005CC290
static int32_t Script_CanEditGuildTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanEditGuildTabInfo(tab)");
    }

    uint32_t tab = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (tab < 6 ? CGGuildInfo::m_rankInfo[player->m_player->m_playerGuildrank].m_bankTabRights[tab] : 0) & 4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: c8b21ff3 manual
// OFFSET: 0x005CB470
static int32_t Script_SetGuildBankTabPermissions(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetGuildTabPermissions(tab, index, enabled)");
    }

    uint8_t tab = static_cast<uint8_t>(lua_tonumber(L, 1)) - 1;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    int32_t enabled = StringToBOOL(L, 3, 0);
    uint32_t rights = tab < 6 ? CGGuildInfo::m_rankInfo[s_guildControlRank].m_bankTabRights[tab] : 0;
    uint32_t flag = s_guildBankTabPermissionFlags[index];

    if (enabled) {
        rights |= flag;
    } else {
        rights &= ~flag;
    }

    CGGuildInfo::m_rankInfo[s_guildControlRank].m_bankTabRights[tab] = rights;
    return 0;
}

// VERIFY: caf993d8 manual
// OFFSET: 0x005CAF40
static int32_t Script_SetGuildRosterSelection(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetGuildRosterSelection(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGGuildInfo::m_numMembers) {
        CGGuildInfo::m_selectedMember = CGGuildInfo::m_roster.m_data[index]->m_guid;
        return 0;
    }

    CGGuildInfo::m_selectedMember.m_guid = 0;
    return 0;
}

// VERIFY: c3aaa605 auto
// OFFSET: 0x005CA190
static int32_t Script_GetGuildRosterMOTD(lua_State* L) {
    lua_pushstring(L, CGGuildInfo::m_motd);
    return 1;
}

// STATIC: 0x00AD0840
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumGuildMembers", &Script_GetNumGuildMembers },
    { "GetGuildRosterMOTD", &Script_GetGuildRosterMOTD },
    { "GetGuildRosterInfo", &Script_GetGuildRosterInfo },
    { "GetGuildRosterLastOnline", &Script_GetGuildRosterLastOnline },
    { "GuildRosterSetPublicNote", &Script_GuildRosterSetPublicNote },
    { "GuildRosterSetOfficerNote", &Script_GuildRosterSetOfficerNote },
    { "SetGuildRosterSelection", &Script_SetGuildRosterSelection },
    { "GetGuildRosterSelection", &Script_GetGuildRosterSelection },
    { "CanGuildPromote", &Script_CanGuildPromote },
    { "CanGuildDemote", &Script_CanGuildDemote },
    { "CanGuildInvite", &Script_CanGuildInvite },
    { "CanGuildRemove", &Script_CanGuildRemove },
    { "CanEditMOTD", &Script_CanEditMOTD },
    { "CanEditPublicNote", &Script_CanEditPublicNote },
    { "CanEditOfficerNote", &Script_CanEditOfficerNote },
    { "CanViewOfficerNote", &Script_CanViewOfficerNote },
    { "CanEditGuildInfo", &Script_CanEditGuildInfo },
    { "CanGuildBankRepair", &Script_CanGuildBankRepair },
    { "CanEditGuildTabInfo", &Script_CanEditGuildTabInfo },
    { "CanEditGuildEvent", &Script_CanEditGuildEvent },
    { "SortGuildRoster", &Script_SortGuildRoster },
    { "SetGuildRosterShowOffline", &Script_SetGuildRosterShowOffline },
    { "GetGuildRosterShowOffline", &Script_GetGuildRosterShowOffline },
    { "GuildControlGetNumRanks", &Script_GuildControlGetNumRanks },
    { "GuildControlGetRankName", &Script_GuildControlGetRankName },
    { "GuildControlSetRank", &Script_GuildControlSetRank },
    { "GuildControlGetRankFlags", &Script_GuildControlGetRankFlags },
    { "GuildControlSetRankFlag", &Script_GuildControlSetRankFlag },
    { "GuildControlSaveRank", &Script_GuildControlSaveRank },
    { "GuildControlAddRank", &Script_GuildControlAddRank },
    { "GuildControlDelRank", &Script_GuildControlDelRank },
    { "SetGuildBankTabPermissions", &Script_SetGuildBankTabPermissions },
    { "GetGuildBankTabPermissions", &Script_GetGuildBankTabPermissions },
    { "SetGuildBankWithdrawLimit", &Script_SetGuildBankWithdrawLimit },
    { "GetGuildBankWithdrawLimit", &Script_GetGuildBankWithdrawLimit },
    { "SetGuildBankTabWithdraw", &Script_SetGuildBankTabWithdraw },
    { "CloseGuildRoster", &Script_CloseGuildRoster },
    { "GuildRoster", &Script_GuildRoster },
    { "GetGuildInfoText", &Script_GetGuildInfoText },
    { "SetGuildInfoText", &Script_SetGuildInfoText },
    { "QueryGuildEventLog", &Script_QueryGuildEventLog },
    { "GetNumGuildEvents", &Script_GetNumGuildEvents },
    { "GetGuildEventInfo", &Script_GetGuildEventInfo },
};

// VERIFY: 361a345a auto
// OFFSET: 0x005CA380
void GuildInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
