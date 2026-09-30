#include "ArenaTeamInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"

// STATIC: 0x00ACF834
int32_t CGArenaTeamInfo::m_showOffline = 1;
// STATIC: 0x00C0F840
ArenaTeam CGArenaTeamInfo::m_arenaTeams[3];


// STATIC: 0x00C0F7F0
uint32_t CGArenaTeamInfo::m_nextAllowedQuery[3];

// STATIC: 0x00C0F7FC
ArenaTeamSort CGArenaTeamInfo::m_sortOrder[7];

// STATIC: 0x00C0F838
WOWGUID CGArenaTeamInfo::m_selectedMember;

// VERIFY: 6e5ba8e8 auto
// OFFSET: 0x005A2D50
int32_t Packet_SMSG_ARENA_TEAM_STATS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t teamId;
    msg->Get(teamId);
    uint32_t index = 0;

    while (CGArenaTeamInfo::m_arenaTeams[index].m_teamId != teamId) {
        if (++index >= 3) {
            return 1;
        }
    }

    ArenaTeam& team = CGArenaTeamInfo::m_arenaTeams[index];
    msg->Get(team.m_rating);
    msg->Get(team.m_weekGames);
    msg->Get(team.m_weekWins);
    msg->Get(team.m_seasonGames);
    msg->Get(team.m_seasonWins);
    msg->Get(team.m_rank);
    FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_TEAM_UPDATE, nullptr);
    return 1;
}

// VERIFY: a2576927 manual
// OFFSET: 0x005A3D60
void ArenaTeam::FreeMemberArray() {
    this->m_teamId = 0;

    for (uint32_t i = 0; i < this->m_members.m_count; i++) {
        delete this->m_members.m_data[i];
        this->m_members.m_data[i] = nullptr;
    }

    this->m_members.m_count = 0;
    this->m_type = 0;
    this->m_numMembers = 0;
    this->m_numOnline = 0;
    this->m_rating = 0;
    this->m_weekGames = 0;
    this->m_weekWins = 0;
    this->m_seasonGames = 0;
    this->m_seasonWins = 0;
    this->m_rank = 0;
}

// VERIFY: d32a70e0 auto
// OFFSET: 0x005A40E0
void CGArenaTeamInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_ROSTER, Packet_SMSG_ARENA_TEAM_ROSTER, nullptr);
    ClientServices::SetMessageHandler(SMSG_ARENA_TEAM_STATS, Packet_SMSG_ARENA_TEAM_STATS, nullptr);

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::m_arenaTeams[i].FreeMemberArray();
    }

    CGArenaTeamInfo::m_selectedMember = WOWGUID();

    for (uint32_t i = 0; i < 7; i++) {
        CGArenaTeamInfo::m_sortOrder[i].m_reverse = 0;
    }

    CGArenaTeamInfo::m_showOffline = 1;

    for (uint32_t i = 0; i < 3; i++) {
        CGArenaTeamInfo::m_nextAllowedQuery[i] = 0;
    }

    for (int32_t i = 0; i < 7; i++) {
        CGArenaTeamInfo::m_sortOrder[i].m_column = i;
    }
}
