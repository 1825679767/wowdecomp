#ifndef WOW_SOURCE_UI_PARTYFRAME_H
#define WOW_SOURCE_UI_PARTYFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGPlayer_C;
struct lua_State;

class CGPartyPetState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint8_t unk_0008[80];
    /* 0058 */ uint8_t m_powerType;
    /* 0059 */ uint8_t unk_0059[3];
    /* 005C */ uint32_t m_health;
    /* 0060 */ uint32_t m_maxHealth;
    /* 0064 */ uint16_t m_power;
    /* 0066 */ uint16_t m_maxPower;
    /* 0068 */ uint8_t unk_0068[320];
};

class CGPartyMemberState {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint16_t m_flags;
    /* 000A */ uint8_t m_powerType;
    /* 000B */ uint8_t unk_000B;
    /* 000C */ uint32_t m_health;
    /* 0010 */ uint32_t m_maxHealth;
    /* 0014 */ uint16_t m_power;
    /* 0016 */ uint16_t m_maxPower;
    /* 0018 */ uint8_t unk_0018[328];
    /* 0160 */ CGPartyPetState m_pet;
    /* 0308 */ uint32_t unk_0308[2];
};

class CGPartyInfo {
    public:
    static void InitializeGame();
    static void CheckClearLFGSlot();
    static void RegisterScriptFunctions();
    static WOWGUID m_members[4];
    static uint32_t m_dungeonDifficulty;
    static WOWGUID m_leader;

    static bool InParty();

    static uint32_t GetNumPartyMembers();
    static int32_t IsMember(const WOWGUID& guid);
    static uint8_t GetPartyAssignments(const WOWGUID& guid);
    static void RemoveAll(int32_t a1);
    static void EnableMember(CGPlayer_C* player, int32_t enable);
    static int32_t IsMemberPet(const WOWGUID& guid, int32_t* index);
    static CGPartyMemberState* GetRemoteStats(const WOWGUID& guid);
    static CGPartyPetState* GetRemotePetStats(const WOWGUID& guid);
};

class CGPartyMemberStateRepository {
    public:
    static CGPartyPetState* FindPetState(const WOWGUID& guid);
    static CGPartyMemberState* GetState(const WOWGUID& guid);
};

extern CGPartyMemberState unk_BD0D08[4];

WOWGUID GetPartyMemberGUID(uint32_t index);
WOWGUID GetPartyPetGUID(uint32_t index);

int32_t Script_GetNumPartyMembers(lua_State* L);
int32_t Script_GetRealNumPartyMembers(lua_State* L);
int32_t Script_GetPartyMember(lua_State* L);
int32_t Script_GetPartyLeaderIndex(lua_State* L);
int32_t Script_IsPartyLeader(lua_State* L);
int32_t Script_IsRealPartyLeader(lua_State* L);
int32_t Script_LeaveParty(lua_State* L);
int32_t Script_GetLootMethod(lua_State* L);
int32_t Script_SetLootMethod(lua_State* L);
int32_t Script_GetLootThreshold(lua_State* L);
int32_t Script_SetLootThreshold(lua_State* L);
int32_t Script_SetPartyAssignment(lua_State* L);
int32_t Script_ClearPartyAssignment(lua_State* L);
int32_t Script_GetPartyAssignment(lua_State* L);
int32_t Script_SilenceMember(lua_State* L);
int32_t Script_UnSilenceMember(lua_State* L);
int32_t Script_SetOptOutOfLoot(lua_State* L);
int32_t Script_GetOptOutOfLoot(lua_State* L);
int32_t Script_CanChangePlayerDifficulty(lua_State* L);
int32_t Script_ChangePlayerDifficulty(lua_State* L);
int32_t Script_IsPartyLFG(lua_State* L);
int32_t Script_HasLFGRestrictions(lua_State* L);

int32_t IsGuidInMyRaid(const WOWGUID& guid);

#endif
