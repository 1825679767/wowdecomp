#include "SkillInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD09B8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumSkillLines", &Script_GetNumSkillLines },
    { "GetSkillLineInfo", &Script_GetSkillLineInfo },
    { "AbandonSkill", &Script_AbandonSkill },
    { "CollapseSkillHeader", &Script_CollapseSkillHeader },
    { "ExpandSkillHeader", &Script_ExpandSkillHeader },
    { "AddSkillUp", &Script_AddSkillUp },
    { "RemoveSkillUp", &Script_RemoveSkillUp },
    { "GetAdjustedSkillPoints", &Script_GetAdjustedSkillPoints },
    { "AcceptSkillUps", &Script_AcceptSkillUps },
    { "CancelSkillUps", &Script_CancelSkillUps },
    { "BuySkillTier", &Script_BuySkillTier },
    { "SetSelectedSkill", &Script_SetSelectedSkill },
    { "GetSelectedSkill", &Script_GetSelectedSkill },
};

// VERIFY: 8f357cd9 auto
// OFFSET: 0x005CD1F0
void SkillInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
