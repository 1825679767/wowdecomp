#include "ClassTrainerFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF3E8
static FrameScript_Method s_ScriptFunctions[] = {
    { "OpenTrainer", &Script_OpenTrainer },
    { "CloseTrainer", &Script_CloseTrainer },
    { "GetNumTrainerServices", &Script_GetNumTrainerServices },
    { "GetTrainerServiceInfo", &Script_GetTrainerServiceInfo },
    { "SelectTrainerService", &Script_SelectTrainerService },
    { "IsTradeskillTrainer", &Script_IsTradeskillTrainer },
    { "GetTrainerSelectionIndex", &Script_GetTrainerSelectionIndex },
    { "GetTrainerGreetingText", &Script_GetTrainerGreetingText },
    { "GetTrainerServiceIcon", &Script_GetTrainerServiceIcon },
    { "GetTrainerServiceSkillLine", &Script_GetTrainerServiceSkillLine },
    { "GetTrainerServiceCost", &Script_GetTrainerServiceCost },
    { "GetTrainerServiceLevelReq", &Script_GetTrainerServiceLevelReq },
    { "GetTrainerServiceSkillReq", &Script_GetTrainerServiceSkillReq },
    { "GetTrainerServiceNumAbilityReq", &Script_GetTrainerServiceNumAbilityReq },
    { "GetTrainerServiceAbilityReq", &Script_GetTrainerServiceAbilityReq },
    { "GetTrainerServiceStepReq", &Script_GetTrainerServiceStepReq },
    { "GetTrainerServiceDescription", &Script_GetTrainerServiceDescription },
    { "IsTrainerServiceSkillStep", &Script_IsTrainerServiceSkillStep },
    { "GetTrainerServiceStepIncrease", &Script_GetTrainerServiceStepIncrease },
    { "BuyTrainerService", &Script_BuyTrainerService },
    { "SetTrainerServiceTypeFilter", &Script_SetTrainerServiceTypeFilter },
    { "SetTrainerSkillLineFilter", &Script_SetTrainerSkillLineFilter },
    { "GetTrainerServiceTypeFilter", &Script_GetTrainerServiceTypeFilter },
    { "GetTrainerSkillLineFilter", &Script_GetTrainerSkillLineFilter },
    { "GetTrainerSkillLines", &Script_GetTrainerSkillLines },
    { "CollapseTrainerSkillLine", &Script_CollapseTrainerSkillLine },
    { "ExpandTrainerSkillLine", &Script_ExpandTrainerSkillLine },
    { "GetTrainerServiceItemLink", &Script_GetTrainerServiceItemLink },
};

// VERIFY: 9493cc0a auto
// OFFSET: 0x00593F70
void ClassTrainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
