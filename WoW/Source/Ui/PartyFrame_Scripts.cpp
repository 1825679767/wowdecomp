#include "PartyFrame.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACC6D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumPartyMembers", &Script_GetNumPartyMembers },
    { "GetRealNumPartyMembers", &Script_GetRealNumPartyMembers },
    { "GetPartyMember", &Script_GetPartyMember },
    { "GetPartyLeaderIndex", &Script_GetPartyLeaderIndex },
    { "IsPartyLeader", &Script_IsPartyLeader },
    { "IsRealPartyLeader", &Script_IsRealPartyLeader },
    { "LeaveParty", &Script_LeaveParty },
    { "GetLootMethod", &Script_GetLootMethod },
    { "SetLootMethod", &Script_SetLootMethod },
    { "GetLootThreshold", &Script_GetLootThreshold },
    { "SetLootThreshold", &Script_SetLootThreshold },
    { "SetPartyAssignment", &Script_SetPartyAssignment },
    { "ClearPartyAssignment", &Script_ClearPartyAssignment },
    { "GetPartyAssignment", &Script_GetPartyAssignment },
    { "SilenceMember", &Script_SilenceMember },
    { "UnSilenceMember", &Script_UnSilenceMember },
    { "SetOptOutOfLoot", &Script_SetOptOutOfLoot },
    { "GetOptOutOfLoot", &Script_GetOptOutOfLoot },
    { "CanChangePlayerDifficulty", &Script_CanChangePlayerDifficulty },
    { "ChangePlayerDifficulty", &Script_ChangePlayerDifficulty },
    { "IsPartyLFG", &Script_IsPartyLFG },
    { "HasLFGRestrictions", &Script_HasLFGRestrictions },
};

// VERIFY: bdeec586 auto
// OFFSET: 0x0052C370
void CGPartyInfo::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: 5a367ac2 manual
// OFFSET: 0x0052C110
int32_t Script_GetNumPartyMembers(lua_State* L) {
    lua_pushnumber(L, CGPartyInfo::GetNumPartyMembers());
    return 1;
}
