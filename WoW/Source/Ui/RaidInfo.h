#ifndef WOW_SOURCE_UI_RAIDINFO_H
#define WOW_SOURCE_UI_RAIDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/PartyFrame.h"

class CGPlayer_C;

uint32_t GetRaidTargetIndexFromGuid(WOWGUID guid);
struct lua_State;

class CGRaidMember {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ int32_t m_rank;
    /* 0010 */ CGPartyMemberState m_stats;
    /* 0320 */ uint32_t unk_0320[4];
};

class CGRaidInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static CGRaidMember* m_members[40];
    static uint32_t m_numMembers;

    static CGPartyMemberState* GetRemoteStats(const WOWGUID& guid);
    static CGPartyPetState* GetRemotePetStats(const WOWGUID& guid);

    static void EnableMember(CGPlayer_C* player, int32_t enable);
    static int32_t IsPetInRaid(const WOWGUID& guid, int32_t* index);
    static int32_t IsPlayerRaidOfficer(const WOWGUID& guid);
    static int32_t IsPVPRaid();
    static uint8_t GetPartyAssignments(const WOWGUID& guid);
    static void GetMapNameWithDifficulty(char* buffer, uint32_t size, int32_t mapId, int32_t difficulty);
};

int32_t Script_GetNumRaidMembers(lua_State* L);
int32_t Script_GetRealNumRaidMembers(lua_State* L);
int32_t Script_GetRaidRosterInfo(lua_State* L);
int32_t Script_SetRaidRosterSelection(lua_State* L);
int32_t Script_GetRaidRosterSelection(lua_State* L);
int32_t Script_IsRaidLeader(lua_State* L);
int32_t Script_IsRealRaidLeader(lua_State* L);
int32_t Script_IsRaidOfficer(lua_State* L);
int32_t Script_SetRaidSubgroup(lua_State* L);
int32_t Script_SwapRaidSubgroup(lua_State* L);
int32_t Script_ConvertToRaid(lua_State* L);
int32_t Script_PromoteToLeader(lua_State* L);
int32_t Script_PromoteToAssistant(lua_State* L);
int32_t Script_DemoteAssistant(lua_State* L);
int32_t Script_SetRaidTarget(lua_State* L);
int32_t Script_GetRaidTargetIndex(lua_State* L);
int32_t Script_DoReadyCheck(lua_State* L);
int32_t Script_ConfirmReadyCheck(lua_State* L);
int32_t Script_GetReadyCheckTimeLeft(lua_State* L);
int32_t Script_GetReadyCheckStatus(lua_State* L);
void RaidInfoRegisterScriptFunctions();

void RaidRosterNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
