#ifndef WOW_SOURCE_UI_GUILDINFO_H
#define WOW_SOURCE_UI_GUILDINFO_H

#include <cstdint>

struct lua_State;

class CGGuildInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static void QueryPermissions();
    static void SetShowOffline(int32_t show);
};

int32_t Script_GetNumGuildMembers(lua_State* L);
int32_t Script_GetGuildRosterMOTD(lua_State* L);
int32_t Script_GetGuildRosterInfo(lua_State* L);
int32_t Script_GetGuildRosterLastOnline(lua_State* L);
int32_t Script_GuildRosterSetPublicNote(lua_State* L);
int32_t Script_GuildRosterSetOfficerNote(lua_State* L);
int32_t Script_SetGuildRosterSelection(lua_State* L);
int32_t Script_GetGuildRosterSelection(lua_State* L);
int32_t Script_CanGuildPromote(lua_State* L);
int32_t Script_CanGuildDemote(lua_State* L);
int32_t Script_CanGuildInvite(lua_State* L);
int32_t Script_CanGuildRemove(lua_State* L);
int32_t Script_CanEditMOTD(lua_State* L);
int32_t Script_CanEditPublicNote(lua_State* L);
int32_t Script_CanEditOfficerNote(lua_State* L);
int32_t Script_CanViewOfficerNote(lua_State* L);
int32_t Script_CanEditGuildInfo(lua_State* L);
int32_t Script_CanGuildBankRepair(lua_State* L);
int32_t Script_CanEditGuildTabInfo(lua_State* L);
int32_t Script_CanEditGuildEvent(lua_State* L);
int32_t Script_SortGuildRoster(lua_State* L);
int32_t Script_SetGuildRosterShowOffline(lua_State* L);
int32_t Script_GetGuildRosterShowOffline(lua_State* L);
int32_t Script_GuildControlGetNumRanks(lua_State* L);
int32_t Script_GuildControlGetRankName(lua_State* L);
int32_t Script_GuildControlSetRank(lua_State* L);
int32_t Script_GuildControlGetRankFlags(lua_State* L);
int32_t Script_GuildControlSetRankFlag(lua_State* L);
int32_t Script_GuildControlSaveRank(lua_State* L);
int32_t Script_GuildControlAddRank(lua_State* L);
int32_t Script_GuildControlDelRank(lua_State* L);
int32_t Script_SetGuildBankTabPermissions(lua_State* L);
int32_t Script_GetGuildBankTabPermissions(lua_State* L);
int32_t Script_SetGuildBankWithdrawLimit(lua_State* L);
int32_t Script_GetGuildBankWithdrawLimit(lua_State* L);
int32_t Script_SetGuildBankTabWithdraw(lua_State* L);
int32_t Script_CloseGuildRoster(lua_State* L);
int32_t Script_GuildRoster(lua_State* L);
int32_t Script_GetGuildInfoText(lua_State* L);
int32_t Script_SetGuildInfoText(lua_State* L);
int32_t Script_QueryGuildEventLog(lua_State* L);
int32_t Script_GetNumGuildEvents(lua_State* L);
int32_t Script_GetGuildEventInfo(lua_State* L);
void GuildInfoRegisterScriptFunctions();

#endif
