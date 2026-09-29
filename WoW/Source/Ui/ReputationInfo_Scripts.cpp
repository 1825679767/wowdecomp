#include "ReputationInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0AE8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFactions", &Script_GetNumFactions },
    { "GetFactionInfo", &Script_GetFactionInfo },
    { "GetFactionInfoByID", &Script_GetFactionInfoByID },
    { "GetWatchedFactionInfo", &Script_GetWatchedFactionInfo },
    { "SetWatchedFactionIndex", &Script_SetWatchedFactionIndex },
    { "FactionToggleAtWar", &Script_FactionToggleAtWar },
    { "CollapseFactionHeader", &Script_CollapseFactionHeader },
    { "CollapseAllFactionHeaders", &Script_CollapseAllFactionHeaders },
    { "SetFactionInactive", &Script_SetFactionInactive },
    { "SetFactionActive", &Script_SetFactionActive },
    { "IsFactionInactive", &Script_IsFactionInactive },
    { "ExpandFactionHeader", &Script_ExpandFactionHeader },
    { "ExpandAllFactionHeaders", &Script_ExpandAllFactionHeaders },
    { "SetSelectedFaction", &Script_SetSelectedFaction },
    { "GetSelectedFaction", &Script_GetSelectedFaction },
};

// VERIFY: 5ad3f6ee auto
// OFFSET: 0x005CFF60
void ReputationInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
