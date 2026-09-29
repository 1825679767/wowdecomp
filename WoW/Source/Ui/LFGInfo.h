#ifndef WOW_SOURCE_UI_LFGINFO_H
#define WOW_SOURCE_UI_LFGINFO_H

#include <cstdint>

struct lua_State;

class CGLookingForGroup {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static void ClearPartyInfo();
    static uint8_t GetClassRoles(uint32_t classID);
};

int32_t Script_SetLFGDungeon(lua_State* L);
int32_t Script_ClearLFGDungeon(lua_State* L);
int32_t Script_ClearAllLFGDungeons(lua_State* L);
int32_t Script_GetLFGInfoLocal(lua_State* L);
int32_t Script_GetLFGInfoServer(lua_State* L);
int32_t Script_GetLFGQueuedList(lua_State* L);
int32_t Script_SetLFGComment(lua_State* L);
int32_t Script_LFGTeleport(lua_State* L);
int32_t Script_JoinLFG(lua_State* L);
int32_t Script_LeaveLFG(lua_State* L);
int32_t Script_RefreshLFGList(lua_State* L);
int32_t Script_SearchLFGJoin(lua_State* L);
int32_t Script_SearchLFGGetJoinedID(lua_State* L);
int32_t Script_SearchLFGLeave(lua_State* L);
int32_t Script_SearchLFGGetNumResults(lua_State* L);
int32_t Script_SearchLFGGetResults(lua_State* L);
int32_t Script_SearchLFGGetEncounterResults(lua_State* L);
int32_t Script_SearchLFGGetPartyResults(lua_State* L);
int32_t Script_SearchLFGSort(lua_State* L);
int32_t Script_GetLFGTypes(lua_State* L);
int32_t Script_GetLFGRoleUpdate(lua_State* L);
int32_t Script_GetLFGRoleUpdateSlot(lua_State* L);
int32_t Script_GetLFGRoleUpdateMember(lua_State* L);
int32_t Script_GetLFGRoles(lua_State* L);
int32_t Script_SetLFGRoles(lua_State* L);
int32_t Script_CompleteLFGRoleCheck(lua_State* L);
int32_t Script_GetLFGProposal(lua_State* L);
int32_t Script_GetLFGProposalMember(lua_State* L);
int32_t Script_GetLFGProposalEncounter(lua_State* L);
int32_t Script_AcceptProposal(lua_State* L);
int32_t Script_RejectProposal(lua_State* L);
int32_t Script_GetLFGBootProposal(lua_State* L);
int32_t Script_SetLFGBootVote(lua_State* L);
int32_t Script_GetLFGQueueStats(lua_State* L);
int32_t Script_GetLastQueueStatusIndex(lua_State* L);
int32_t Script_GetLFGDungeonInfo(lua_State* L);
int32_t Script_GetLFGRandomDungeonInfo(lua_State* L);
int32_t Script_IsLFGDungeonJoinable(lua_State* L);
int32_t Script_SetLFGHeaderCollapsed(lua_State* L);
int32_t Script_SetLFGDungeonEnabled(lua_State* L);
int32_t Script_GetLFGCompletionReward(lua_State* L);
int32_t Script_GetLFGCompletionRewardItem(lua_State* L);
int32_t Script_IsInLFGDungeon(lua_State* L);
int32_t Script_CanPartyLFGBackfill(lua_State* L);
int32_t Script_GetPartyLFGBackfillInfo(lua_State* L);
int32_t Script_PartyLFGStartBackfill(lua_State* L);
int32_t Script_GetLFGRandomCooldownExpiration(lua_State* L);
int32_t Script_UnitHasLFGRandomCooldown(lua_State* L);
int32_t Script_GetLFGDeserterExpiration(lua_State* L);
int32_t Script_UnitHasLFGDeserter(lua_State* L);
int32_t Script_GetAvailableRoles(lua_State* L);
int32_t Script_GetLFDChoiceOrder(lua_State* L);
int32_t Script_GetLFDChoiceInfo(lua_State* L);
int32_t Script_GetLFDChoiceCollapseState(lua_State* L);
int32_t Script_GetLFDChoiceEnabledState(lua_State* L);
int32_t Script_GetLFDChoiceLockedState(lua_State* L);
int32_t Script_GetLFDLockPlayerCount(lua_State* L);
int32_t Script_GetLFDLockInfo(lua_State* L);
int32_t Script_RequestLFDPlayerLockInfo(lua_State* L);
int32_t Script_RequestLFDPartyLockInfo(lua_State* L);
int32_t Script_GetNumRandomDungeons(lua_State* L);
int32_t Script_GetLFGDungeonRewards(lua_State* L);
int32_t Script_GetLFGDungeonRewardInfo(lua_State* L);
int32_t Script_GetLFGDungeonRewardLink(lua_State* L);
int32_t Script_GetRandomDungeonBestChoice(lua_State* L);
int32_t Script_GetLFRChoiceOrder(lua_State* L);
int32_t Script_IsListedInLFR(lua_State* L);
void LookingForGroupRegisterScriptFunctions();

#endif
