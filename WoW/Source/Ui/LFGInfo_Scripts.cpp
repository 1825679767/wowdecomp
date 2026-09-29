#include "LFGInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACD668
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetLFGDungeon", &Script_SetLFGDungeon },
    { "ClearLFGDungeon", &Script_ClearLFGDungeon },
    { "ClearAllLFGDungeons", &Script_ClearAllLFGDungeons },
    { "GetLFGInfoLocal", &Script_GetLFGInfoLocal },
    { "GetLFGInfoServer", &Script_GetLFGInfoServer },
    { "GetLFGQueuedList", &Script_GetLFGQueuedList },
    { "SetLFGComment", &Script_SetLFGComment },
    { "LFGTeleport", &Script_LFGTeleport },
    { "JoinLFG", &Script_JoinLFG },
    { "LeaveLFG", &Script_LeaveLFG },
    { "RefreshLFGList", &Script_RefreshLFGList },
    { "SearchLFGJoin", &Script_SearchLFGJoin },
    { "SearchLFGGetJoinedID", &Script_SearchLFGGetJoinedID },
    { "SearchLFGLeave", &Script_SearchLFGLeave },
    { "SearchLFGGetNumResults", &Script_SearchLFGGetNumResults },
    { "SearchLFGGetResults", &Script_SearchLFGGetResults },
    { "SearchLFGGetEncounterResults", &Script_SearchLFGGetEncounterResults },
    { "SearchLFGGetPartyResults", &Script_SearchLFGGetPartyResults },
    { "SearchLFGSort", &Script_SearchLFGSort },
    { "GetLFGTypes", &Script_GetLFGTypes },
    { "GetLFGRoleUpdate", &Script_GetLFGRoleUpdate },
    { "GetLFGRoleUpdateSlot", &Script_GetLFGRoleUpdateSlot },
    { "GetLFGRoleUpdateMember", &Script_GetLFGRoleUpdateMember },
    { "GetLFGRoles", &Script_GetLFGRoles },
    { "SetLFGRoles", &Script_SetLFGRoles },
    { "CompleteLFGRoleCheck", &Script_CompleteLFGRoleCheck },
    { "GetLFGProposal", &Script_GetLFGProposal },
    { "GetLFGProposalMember", &Script_GetLFGProposalMember },
    { "GetLFGProposalEncounter", &Script_GetLFGProposalEncounter },
    { "AcceptProposal", &Script_AcceptProposal },
    { "RejectProposal", &Script_RejectProposal },
    { "GetLFGBootProposal", &Script_GetLFGBootProposal },
    { "SetLFGBootVote", &Script_SetLFGBootVote },
    { "GetLFGQueueStats", &Script_GetLFGQueueStats },
    { "GetLastQueueStatusIndex", &Script_GetLastQueueStatusIndex },
    { "GetLFGDungeonInfo", &Script_GetLFGDungeonInfo },
    { "GetLFGRandomDungeonInfo", &Script_GetLFGRandomDungeonInfo },
    { "IsLFGDungeonJoinable", &Script_IsLFGDungeonJoinable },
    { "SetLFGHeaderCollapsed", &Script_SetLFGHeaderCollapsed },
    { "SetLFGDungeonEnabled", &Script_SetLFGDungeonEnabled },
    { "GetLFGCompletionReward", &Script_GetLFGCompletionReward },
    { "GetLFGCompletionRewardItem", &Script_GetLFGCompletionRewardItem },
    { "IsInLFGDungeon", &Script_IsInLFGDungeon },
    { "CanPartyLFGBackfill", &Script_CanPartyLFGBackfill },
    { "GetPartyLFGBackfillInfo", &Script_GetPartyLFGBackfillInfo },
    { "PartyLFGStartBackfill", &Script_PartyLFGStartBackfill },
    { "GetLFGRandomCooldownExpiration", &Script_GetLFGRandomCooldownExpiration },
    { "UnitHasLFGRandomCooldown", &Script_UnitHasLFGRandomCooldown },
    { "GetLFGDeserterExpiration", &Script_GetLFGDeserterExpiration },
    { "UnitHasLFGDeserter", &Script_UnitHasLFGDeserter },
    { "GetAvailableRoles", &Script_GetAvailableRoles },
    { "GetLFDChoiceOrder", &Script_GetLFDChoiceOrder },
    { "GetLFDChoiceInfo", &Script_GetLFDChoiceInfo },
    { "GetLFDChoiceCollapseState", &Script_GetLFDChoiceCollapseState },
    { "GetLFDChoiceEnabledState", &Script_GetLFDChoiceEnabledState },
    { "GetLFDChoiceLockedState", &Script_GetLFDChoiceLockedState },
    { "GetLFDLockPlayerCount", &Script_GetLFDLockPlayerCount },
    { "GetLFDLockInfo", &Script_GetLFDLockInfo },
    { "RequestLFDPlayerLockInfo", &Script_RequestLFDPlayerLockInfo },
    { "RequestLFDPartyLockInfo", &Script_RequestLFDPartyLockInfo },
    { "GetNumRandomDungeons", &Script_GetNumRandomDungeons },
    { "GetLFGDungeonRewards", &Script_GetLFGDungeonRewards },
    { "GetLFGDungeonRewardInfo", &Script_GetLFGDungeonRewardInfo },
    { "GetLFGDungeonRewardLink", &Script_GetLFGDungeonRewardLink },
    { "GetRandomDungeonBestChoice", &Script_GetRandomDungeonBestChoice },
    { "GetLFRChoiceOrder", &Script_GetLFRChoiceOrder },
    { "IsListedInLFR", &Script_IsListedInLFR },
};

// VERIFY: cc0f9700 auto
// OFFSET: 0x005531D0
void LookingForGroupRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
