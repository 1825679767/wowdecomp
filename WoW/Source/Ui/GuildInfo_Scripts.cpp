#include "GuildInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
