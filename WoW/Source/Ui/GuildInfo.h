#ifndef WOW_SOURCE_UI_GUILDINFO_H
#define WOW_SOURCE_UI_GUILDINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

struct GuildRankInfo {
    /* 0000 */ uint32_t m_rights;
    /* 0004 */ int32_t m_bankMoneyPerDay;
    /* 0008 */ uint32_t m_bankTabRights[6];
    /* 0020 */ int32_t m_bankTabStacksPerDay[6];
};

struct GuildMemberInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name[48];
    /* 0038 */ int32_t m_level;
    /* 003C */ int32_t m_class;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t m_zoneID;
    /* 0048 */ int32_t m_rank;
    /* 004C */ int32_t m_online;
    /* 0050 */ uint8_t m_status;
    /* 0051 */ uint8_t unk_0051;
    /* 0052 */ char m_note[128];
    /* 00D2 */ char m_officerNote[128];
    /* 0152 */ uint8_t unk_0152[2];
    /* 0154 */ float m_lastOnline;
};

struct GuildRosterSortCriteria {
    /* 0000 */ int32_t m_column;
    /* 0004 */ int32_t m_descending;
};

struct GuildRankFlag {
    /* 0000 */ uint32_t m_mask;
    /* 0004 */ int32_t m_enabled;
};

class CGGuildInfo {
    public:
    static char m_infoText[2004];
    static char m_motd[516];
    static GuildRosterSortCriteria m_sortCriteria[8];
    static uint32_t m_numMembers;
    static uint32_t m_numOnlineMembers;
    static int32_t m_showOffline;
    static WOWGUID m_selectedMember;
    static int32_t unk_C22AC8;
    static int32_t unk_C22AD0;
    static uint8_t unk_C22AD4;
    static TSGrowableArray<GuildMemberInfo*> m_roster;
    static GuildRankInfo m_rankInfo[10];
    static int32_t m_numEventLogEntries;
    static uint32_t m_numRanks;
    static void EnterWorld();
    static void InitializeGame();
    static void QueryPermissions();
    static void SetShowOffline(int32_t show);
};

extern int32_t s_guildControlRank;

int32_t Packet_SMSG_GUILD_ROSTER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_PERMISSIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_EVENT_LOG_QUERY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t QSortRoster(const void* a, const void* b);

int32_t Script_GetNumGuildMembers(lua_State* L);
int32_t Script_GetGuildRosterInfo(lua_State* L);
int32_t Script_GetGuildRosterLastOnline(lua_State* L);
int32_t Script_GuildRosterSetPublicNote(lua_State* L);
int32_t Script_GuildRosterSetOfficerNote(lua_State* L);
int32_t Script_GetGuildRosterSelection(lua_State* L);
int32_t Script_SortGuildRoster(lua_State* L);
int32_t Script_GuildControlGetRankName(lua_State* L);
int32_t Script_GuildControlSetRank(lua_State* L);
int32_t Script_GuildControlSaveRank(lua_State* L);
int32_t Script_GuildControlAddRank(lua_State* L);
int32_t Script_GetGuildBankTabPermissions(lua_State* L);
int32_t Script_CloseGuildRoster(lua_State* L);
int32_t Script_GuildRoster(lua_State* L);
int32_t Script_GetGuildInfoText(lua_State* L);
int32_t Script_SetGuildInfoText(lua_State* L);
int32_t Script_GetGuildEventInfo(lua_State* L);
void GuildInfoRegisterScriptFunctions();

#endif
