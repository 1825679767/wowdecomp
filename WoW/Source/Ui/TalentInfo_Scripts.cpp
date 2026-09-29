#include "TalentInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD05F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumTalentTabs", &Script_GetNumTalentTabs },
    { "GetTalentTabInfo", &Script_GetTalentTabInfo },
    { "GetNumTalents", &Script_GetNumTalents },
    { "GetTalentInfo", &Script_GetTalentInfo },
    { "GetTalentLink", &Script_GetTalentLink },
    { "GetTalentPrereqs", &Script_GetTalentPrereqs },
    { "LearnTalent", &Script_LearnTalent },
    { "GetUnspentTalentPoints", &Script_GetUnspentTalentPoints },
    { "GetNumTalentGroups", &Script_GetNumTalentGroups },
    { "GetActiveTalentGroup", &Script_GetActiveTalentGroup },
    { "SetActiveTalentGroup", &Script_SetActiveTalentGroup },
    { "GetPreviewTalentPointsSpent", &Script_GetPreviewTalentPointsSpent },
    { "GetGroupPreviewTalentPointsSpent", &Script_GetGroupPreviewTalentPointsSpent },
    { "AddPreviewTalentPoints", &Script_AddPreviewTalentPoints },
    { "ResetPreviewTalentPoints", &Script_ResetPreviewTalentPoints },
    { "ResetGroupPreviewTalentPoints", &Script_ResetGroupPreviewTalentPoints },
    { "LearnPreviewTalents", &Script_LearnPreviewTalents },
};

// VERIFY: c3db1349 auto
// OFFSET: 0x005C5880
void TalentInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
