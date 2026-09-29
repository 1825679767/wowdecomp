#include "BattlefieldInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACD178
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumBattlefields", &Script_GetNumBattlefields },
    { "GetBattlefieldInfo", &Script_GetBattlefieldInfo },
    { "GetBattlefieldInstanceInfo", &Script_GetBattlefieldInstanceInfo },
    { "IsBattlefieldArena", &Script_IsBattlefieldArena },
    { "IsActiveBattlefieldArena", &Script_IsActiveBattlefieldArena },
    { "JoinBattlefield", &Script_JoinBattlefield },
    { "SetSelectedBattlefield", &Script_SetSelectedBattlefield },
    { "GetSelectedBattlefield", &Script_GetSelectedBattlefield },
    { "AcceptBattlefieldPort", &Script_AcceptBattlefieldPort },
    { "GetBattlefieldStatus", &Script_GetBattlefieldStatus },
    { "GetBattlefieldPortExpiration", &Script_GetBattlefieldPortExpiration },
    { "GetBattlefieldInstanceExpiration", &Script_GetBattlefieldInstanceExpiration },
    { "GetBattlefieldInstanceRunTime", &Script_GetBattlefieldInstanceRunTime },
    { "GetBattlefieldEstimatedWaitTime", &Script_GetBattlefieldEstimatedWaitTime },
    { "GetBattlefieldTimeWaited", &Script_GetBattlefieldTimeWaited },
    { "CloseBattlefield", &Script_CloseBattlefield },
    { "RequestBattlefieldScoreData", &Script_RequestBattlefieldScoreData },
    { "GetNumBattlefieldScores", &Script_GetNumBattlefieldScores },
    { "GetBattlefieldScore", &Script_GetBattlefieldScore },
    { "GetBattlefieldWinner", &Script_GetBattlefieldWinner },
    { "SetBattlefieldScoreFaction", &Script_SetBattlefieldScoreFaction },
    { "LeaveBattlefield", &Script_LeaveBattlefield },
    { "GetNumBattlefieldStats", &Script_GetNumBattlefieldStats },
    { "GetBattlefieldStatInfo", &Script_GetBattlefieldStatInfo },
    { "GetBattlefieldStatData", &Script_GetBattlefieldStatData },
    { "RequestBattlefieldPositions", &Script_RequestBattlefieldPositions },
    { "GetNumBattlefieldPositions", &Script_GetNumBattlefieldPositions },
    { "GetBattlefieldPosition", &Script_GetBattlefieldPosition },
    { "GetNumBattlefieldFlagPositions", &Script_GetNumBattlefieldFlagPositions },
    { "GetBattlefieldFlagPosition", &Script_GetBattlefieldFlagPosition },
    { "GetNumBattlefieldVehicles", &Script_GetNumBattlefieldVehicles },
    { "GetBattlefieldVehicleInfo", &Script_GetBattlefieldVehicleInfo },
    { "CanJoinBattlefieldAsGroup", &Script_CanJoinBattlefieldAsGroup },
    { "GetBattlefieldMapIconScale", &Script_GetBattlefieldMapIconScale },
    { "GetBattlefieldTeamInfo", &Script_GetBattlefieldTeamInfo },
    { "GetBattlefieldArenaFaction", &Script_GetBattlefieldArenaFaction },
    { "SortBattlefieldScoreData", &Script_SortBattlefieldScoreData },
    { "HearthAndResurrectFromArea", &Script_HearthAndResurrectFromArea },
    { "CanHearthAndResurrectFromArea", &Script_CanHearthAndResurrectFromArea },
    { "GetNumBattlegroundTypes", &Script_GetNumBattlegroundTypes },
    { "GetBattlegroundInfo", &Script_GetBattlegroundInfo },
    { "RequestBattlegroundInstanceInfo", &Script_RequestBattlegroundInstanceInfo },
    { "GetNumArenaOpponents", &Script_GetNumArenaOpponents },
    { "BattlefieldMgrEntryInviteResponse", &Script_BattlefieldMgrEntryInviteResponse },
    { "BattlefieldMgrQueueRequest", &Script_BattlefieldMgrQueueRequest },
    { "BattlefieldMgrQueueInviteResponse", &Script_BattlefieldMgrQueueInviteResponse },
    { "BattlefieldMgrExitRequest", &Script_BattlefieldMgrExitRequest },
    { "GetWorldPVPQueueStatus", &Script_GetWorldPVPQueueStatus },
    { "GetHolidayBGHonorCurrencyBonuses", &Script_GetHolidayBGHonorCurrencyBonuses },
    { "GetRandomBGHonorCurrencyBonuses", &Script_GetRandomBGHonorCurrencyBonuses },
    { "SortBGList", &Script_SortBGList },
};

// VERIFY: 59f906df auto
// OFFSET: 0x0054A3E0
void BattlefieldInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
