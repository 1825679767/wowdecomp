#include "RaidInfo.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACE790
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumRaidMembers", &Script_GetNumRaidMembers },
    { "GetRealNumRaidMembers", &Script_GetRealNumRaidMembers },
    { "GetRaidRosterInfo", &Script_GetRaidRosterInfo },
    { "SetRaidRosterSelection", &Script_SetRaidRosterSelection },
    { "GetRaidRosterSelection", &Script_GetRaidRosterSelection },
    { "IsRaidLeader", &Script_IsRaidLeader },
    { "IsRealRaidLeader", &Script_IsRealRaidLeader },
    { "IsRaidOfficer", &Script_IsRaidOfficer },
    { "SetRaidSubgroup", &Script_SetRaidSubgroup },
    { "SwapRaidSubgroup", &Script_SwapRaidSubgroup },
    { "ConvertToRaid", &Script_ConvertToRaid },
    { "PromoteToLeader", &Script_PromoteToLeader },
    { "PromoteToAssistant", &Script_PromoteToAssistant },
    { "DemoteAssistant", &Script_DemoteAssistant },
    { "SetRaidTarget", &Script_SetRaidTarget },
    { "GetRaidTargetIndex", &Script_GetRaidTargetIndex },
    { "DoReadyCheck", &Script_DoReadyCheck },
    { "ConfirmReadyCheck", &Script_ConfirmReadyCheck },
    { "GetReadyCheckTimeLeft", &Script_GetReadyCheckTimeLeft },
    { "GetReadyCheckStatus", &Script_GetReadyCheckStatus },
};

// VERIFY: 8b29d591 auto
// OFFSET: 0x00572D10
void RaidInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: b1ed02bf manual
// OFFSET: 0x00572B40
int32_t Script_GetNumRaidMembers(lua_State* L) {
    lua_pushnumber(L, CGRaidInfo::m_numMembers);
    return 1;
}
