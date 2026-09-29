#ifndef WOW_SOURCE_UI_BATTLEFIELDINFO_H
#define WOW_SOURCE_UI_BATTLEFIELDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGPartyMemberState;
class CGPartyPetState;
struct lua_State;

class CGBattlefieldInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static WOWGUID m_arenaOpponentPets[5][2];
    static WOWGUID m_arenaOpponents[5];
    static int32_t m_instanceType;
    static uint8_t m_arenaFaction;
    static int32_t m_arenaOpponentSeen[5];

    static void AddArenaOpponent(const WOWGUID& guid);
    static void AddVehicle(const WOWGUID& guid);
    static void AddArenaOpponentPet(const WOWGUID& petGuid, const WOWGUID& ownerGuid);
    static WOWGUID GetArenaOpponent(uint32_t index);
    static CGPartyMemberState* GetRemoteStats(const WOWGUID& guid);
    static CGPartyPetState* GetRemotePetStats(const WOWGUID& guid);
    static WOWGUID GetArenaOpponentPet(uint32_t index);
    static int32_t GetArenaOpponentIndex(const WOWGUID& guid, int32_t* index);
    static void RemoveVehicle(const WOWGUID& guid);
};

uint32_t GetArenaPetGUIDCount();

int32_t Script_GetNumBattlefields(lua_State* L);
int32_t Script_GetBattlefieldInfo(lua_State* L);
int32_t Script_GetBattlefieldInstanceInfo(lua_State* L);
int32_t Script_IsBattlefieldArena(lua_State* L);
int32_t Script_IsActiveBattlefieldArena(lua_State* L);
int32_t Script_JoinBattlefield(lua_State* L);
int32_t Script_SetSelectedBattlefield(lua_State* L);
int32_t Script_GetSelectedBattlefield(lua_State* L);
int32_t Script_AcceptBattlefieldPort(lua_State* L);
int32_t Script_GetBattlefieldStatus(lua_State* L);
int32_t Script_GetBattlefieldPortExpiration(lua_State* L);
int32_t Script_GetBattlefieldInstanceExpiration(lua_State* L);
int32_t Script_GetBattlefieldInstanceRunTime(lua_State* L);
int32_t Script_GetBattlefieldEstimatedWaitTime(lua_State* L);
int32_t Script_GetBattlefieldTimeWaited(lua_State* L);
int32_t Script_CloseBattlefield(lua_State* L);
int32_t Script_RequestBattlefieldScoreData(lua_State* L);
int32_t Script_GetNumBattlefieldScores(lua_State* L);
int32_t Script_GetBattlefieldScore(lua_State* L);
int32_t Script_GetBattlefieldWinner(lua_State* L);
int32_t Script_SetBattlefieldScoreFaction(lua_State* L);
int32_t Script_LeaveBattlefield(lua_State* L);
int32_t Script_GetNumBattlefieldStats(lua_State* L);
int32_t Script_GetBattlefieldStatInfo(lua_State* L);
int32_t Script_GetBattlefieldStatData(lua_State* L);
int32_t Script_RequestBattlefieldPositions(lua_State* L);
int32_t Script_GetNumBattlefieldPositions(lua_State* L);
int32_t Script_GetBattlefieldPosition(lua_State* L);
int32_t Script_GetNumBattlefieldFlagPositions(lua_State* L);
int32_t Script_GetBattlefieldFlagPosition(lua_State* L);
int32_t Script_GetNumBattlefieldVehicles(lua_State* L);
int32_t Script_GetBattlefieldVehicleInfo(lua_State* L);
int32_t Script_CanJoinBattlefieldAsGroup(lua_State* L);
int32_t Script_GetBattlefieldMapIconScale(lua_State* L);
int32_t Script_GetBattlefieldTeamInfo(lua_State* L);
int32_t Script_GetBattlefieldArenaFaction(lua_State* L);
int32_t Script_SortBattlefieldScoreData(lua_State* L);
int32_t Script_HearthAndResurrectFromArea(lua_State* L);
int32_t Script_CanHearthAndResurrectFromArea(lua_State* L);
int32_t Script_GetNumBattlegroundTypes(lua_State* L);
int32_t Script_GetBattlegroundInfo(lua_State* L);
int32_t Script_RequestBattlegroundInstanceInfo(lua_State* L);
int32_t Script_GetNumArenaOpponents(lua_State* L);
int32_t Script_BattlefieldMgrEntryInviteResponse(lua_State* L);
int32_t Script_BattlefieldMgrQueueRequest(lua_State* L);
int32_t Script_BattlefieldMgrQueueInviteResponse(lua_State* L);
int32_t Script_BattlefieldMgrExitRequest(lua_State* L);
int32_t Script_GetWorldPVPQueueStatus(lua_State* L);
int32_t Script_GetHolidayBGHonorCurrencyBonuses(lua_State* L);
int32_t Script_GetRandomBGHonorCurrencyBonuses(lua_State* L);
int32_t Script_SortBGList(lua_State* L);
void BattlefieldInfoRegisterScriptFunctions();

#endif
