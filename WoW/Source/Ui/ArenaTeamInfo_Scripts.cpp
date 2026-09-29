#include "ArenaTeamInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF838
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetArenaTeam", &Script_GetArenaTeam },
    { "GetNumArenaTeamMembers", &Script_GetNumArenaTeamMembers },
    { "GetArenaTeamRosterInfo", &Script_GetArenaTeamRosterInfo },
    { "GetArenaTeamGdfInfo", &Script_GetArenaTeamGdfInfo },
    { "SetArenaTeamRosterSelection", &Script_SetArenaTeamRosterSelection },
    { "GetArenaTeamRosterSelection", &Script_GetArenaTeamRosterSelection },
    { "SortArenaTeamRoster", &Script_SortArenaTeamRoster },
    { "SetArenaTeamRosterShowOffline", &Script_SetArenaTeamRosterShowOffline },
    { "GetArenaTeamRosterShowOffline", &Script_GetArenaTeamRosterShowOffline },
    { "CloseArenaTeamRoster", &Script_CloseArenaTeamRoster },
    { "ArenaTeamRoster", &Script_ArenaTeamRoster },
    { "GetCurrentArenaSeason", &Script_GetCurrentArenaSeason },
    { "GetPreviousArenaSeason", &Script_GetPreviousArenaSeason },
};

// VERIFY: 2e34f30b auto
// OFFSET: 0x005A2AA0
void ArenaTeamInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
