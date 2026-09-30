#ifndef WOW_SOURCE_UI_PARTYFRAME_H
#define WOW_SOURCE_UI_PARTYFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
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
    /* 0018 */ uint16_t m_level;
    /* 001A */ uint16_t m_zoneID;
    /* 001C */ int16_t m_posX;
    /* 001E */ int16_t m_posY;
    /* 0020 */ uint8_t unk_0020[320];
    /* 0160 */ CGPartyPetState m_pet;
    /* 0308 */ uint32_t unk_0308[2];
};

class CGPartyInfo {
    public:
    static uint32_t m_memberRoles[4];
    static uint32_t unk_BD0D00;
    static uint32_t unk_BD1984;
    static uint8_t unk_BD1988;
    static uint8_t unk_BD1989;
    static uint8_t m_playerRole;
    static void InitializeGame();
    static void CheckClearLFGSlot();
    static void RegisterScriptFunctions();
    static WOWGUID m_members[4];
    static uint32_t m_dungeonDifficulty;
    static uint32_t m_raidDifficulty;
    static int32_t m_dynamicDifficulty;
    static int32_t m_lfgRestricted;
    static int32_t m_leaderIndex;
    static int32_t m_lootThreshold;
    static uint32_t m_realNumMembers;
    static int32_t m_optOutOfLoot;
    static int32_t m_lfgSlot;
    static WOWGUID m_leader;
    static WOWGUID m_realLeader;
    static int32_t m_lootMethod;
    static WOWGUID m_lootMaster;

    static bool InParty();

    static uint32_t GetNumPartyMembers();
    static uint32_t GetGroupRole(const WOWGUID& guid);
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

int32_t Script_SetLootMethod(lua_State* L);
int32_t Script_SetPartyAssignment(lua_State* L);
int32_t Script_ClearPartyAssignment(lua_State* L);
int32_t Script_GetPartyAssignment(lua_State* L);
int32_t Script_SilenceMember(lua_State* L);
int32_t Script_UnSilenceMember(lua_State* L);
int32_t Script_SetOptOutOfLoot(lua_State* L);
int32_t Script_CanChangePlayerDifficulty(lua_State* L);
int32_t Script_ChangePlayerDifficulty(lua_State* L);

int32_t IsGuidInMyRaid(const WOWGUID& guid);

int32_t Packet_SMSG_INSTANCE_SAVE_CREATED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAYER_DIFFICULTY_CHANGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t CCommand_SetDifficulty(const char* command, const char* arguments);

#endif
