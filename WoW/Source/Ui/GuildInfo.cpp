#include "GuildInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// STATIC: 0x00C22ACC
int32_t CGGuildInfo::m_numEventLogEntries;

// STATIC: 0x00C22AB8
uint32_t CGGuildInfo::m_numRanks;
// STATIC: 0x00C21E60
GuildRankInfo CGGuildInfo::m_rankInfo[10];

// STATIC: 0x00C22090
char CGGuildInfo::m_infoText[2004];

// STATIC: 0x00C22868
char CGGuildInfo::m_motd[516];

// STATIC: 0x00C22A70
GuildRosterSortCriteria CGGuildInfo::m_sortCriteria[8];

// STATIC: 0x00C22AB0
uint32_t CGGuildInfo::m_numMembers;

// STATIC: 0x00C22AB4
uint32_t CGGuildInfo::m_numOnlineMembers;

// STATIC: 0x00C22ABC
int32_t CGGuildInfo::m_showOffline;

// STATIC: 0x00C22AC0
WOWGUID CGGuildInfo::m_selectedMember;

// STATIC: 0x00C22AC8
int32_t CGGuildInfo::unk_C22AC8;

// STATIC: 0x00C22AD0
int32_t CGGuildInfo::unk_C22AD0;

// STATIC: 0x00C22AD4
uint8_t CGGuildInfo::unk_C22AD4;

// STATIC: 0x00C22AD8
TSGrowableArray<GuildMemberInfo*> CGGuildInfo::m_roster;

// STATIC: 0x00AD07A8
int32_t s_guildControlRank = -1;


// VERIFY: 3d0e376e manual
// OFFSET: 0x005CAA40
void CGGuildInfo::QueryPermissions() {
    CDataStore permissions;
    permissions.Put(static_cast<uint32_t>(MSG_GUILD_PERMISSIONS));
    permissions.Finalize();
    ClientServices::Send2(&permissions);

    CDataStore withdrawn;
    withdrawn.Put(static_cast<uint32_t>(MSG_GUILD_BANK_MONEY_WITHDRAWN));
    withdrawn.Finalize();
    ClientServices::Send2(&withdrawn);
}

// VERIFY: f0cf42f9 auto
// OFFSET: 0x005CD0B0
void CGGuildInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GUILD_ROSTER, Packet_SMSG_GUILD_ROSTER, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_PERMISSIONS, Packet_MSG_GUILD_PERMISSIONS, nullptr);
    ClientServices::SetMessageHandler(MSG_GUILD_EVENT_LOG_QUERY, Packet_MSG_GUILD_EVENT_LOG_QUERY, nullptr);

    CGGuildInfo::m_numMembers = 0;
    CGGuildInfo::m_numOnlineMembers = 0;
    CGGuildInfo::m_numRanks = 0;
    CGGuildInfo::m_showOffline = 0;
    CGGuildInfo::m_selectedMember.m_guid = 0;
    CGGuildInfo::unk_C22AC8 = 0;
    CGGuildInfo::m_numEventLogEntries = 0;
    CGGuildInfo::unk_C22AD0 = 0;
    CGGuildInfo::m_infoText[0] = '\0';
    CGGuildInfo::m_motd[0] = '\0';

    for (int32_t i = 0; i < 8; i++) {
        CGGuildInfo::m_sortCriteria[i].m_column = i;
        CGGuildInfo::m_sortCriteria[i].m_descending = 0;
    }

    s_guildControlRank = -1;
}

// VERIFY: 6089fd22 auto
// OFFSET: 0x005CA870
void CGGuildInfo::SetShowOffline(int32_t show) {
    if (show == CGGuildInfo::m_showOffline) {
        return;
    }

    CGGuildInfo::m_showOffline = show;

    if (CGGuildInfo::m_numMembers) {
        qsort(CGGuildInfo::m_roster.m_data, CGGuildInfo::m_numMembers, sizeof(GuildMemberInfo*), QSortRoster);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_ROSTER_UPDATE, nullptr);
    }
}

// VERIFY: 504618a5 manual
// OFFSET: 0x005CB900
void CGGuildInfo::EnterWorld() {
    CGGuildInfo::m_infoText[0] = '\0';
    CGGuildInfo::m_motd[0] = '\0';

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_GUILD_PERMISSIONS));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    for (int32_t i = 0; i < 6; i++) {
        CGGuildInfo::m_rankInfo[0].m_bankTabRights[i] = -1;
        CGGuildInfo::m_rankInfo[0].m_bankTabStacksPerDay[i] = -1;
    }
}
