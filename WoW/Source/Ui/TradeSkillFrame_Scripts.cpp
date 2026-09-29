#include "TradeSkillFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0E80
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTradeSkill", &Script_CloseTradeSkill },
    { "GetNumTradeSkills", &Script_GetNumTradeSkills },
    { "GetTradeSkillInfo", &Script_GetTradeSkillInfo },
    { "SelectTradeSkill", &Script_SelectTradeSkill },
    { "GetTradeSkillSelectionIndex", &Script_GetTradeSkillSelectionIndex },
    { "GetTradeSkillCooldown", &Script_GetTradeSkillCooldown },
    { "GetTradeSkillIcon", &Script_GetTradeSkillIcon },
    { "GetTradeSkillNumMade", &Script_GetTradeSkillNumMade },
    { "GetTradeSkillLine", &Script_GetTradeSkillLine },
    { "GetTradeSkillItemLink", &Script_GetTradeSkillItemLink },
    { "SetTradeSkillItemNameFilter", &Script_SetTradeSkillItemNameFilter },
    { "GetTradeSkillItemNameFilter", &Script_GetTradeSkillItemNameFilter },
    { "SetTradeSkillItemLevelFilter", &Script_SetTradeSkillItemLevelFilter },
    { "GetTradeSkillItemLevelFilter", &Script_GetTradeSkillItemLevelFilter },
    { "GetTradeSkillNumReagents", &Script_GetTradeSkillNumReagents },
    { "GetTradeSkillReagentInfo", &Script_GetTradeSkillReagentInfo },
    { "GetTradeSkillReagentItemLink", &Script_GetTradeSkillReagentItemLink },
    { "GetTradeSkillTools", &Script_GetTradeSkillTools },
    { "GetTradeSkillDescription", &Script_GetTradeSkillDescription },
    { "GetTradeSkillSubClasses", &Script_GetTradeSkillSubClasses },
    { "GetTradeSkillInvSlots", &Script_GetTradeSkillInvSlots },
    { "SetTradeSkillSubClassFilter", &Script_SetTradeSkillSubClassFilter },
    { "GetTradeSkillSubClassFilter", &Script_GetTradeSkillSubClassFilter },
    { "SetTradeSkillInvSlotFilter", &Script_SetTradeSkillInvSlotFilter },
    { "GetTradeSkillInvSlotFilter", &Script_GetTradeSkillInvSlotFilter },
    { "TradeSkillOnlyShowMakeable", &Script_TradeSkillOnlyShowMakeable },
    { "TradeSkillOnlyShowSkillUps", &Script_TradeSkillOnlyShowSkillUps },
    { "CollapseTradeSkillSubClass", &Script_CollapseTradeSkillSubClass },
    { "ExpandTradeSkillSubClass", &Script_ExpandTradeSkillSubClass },
    { "GetFirstTradeSkill", &Script_GetFirstTradeSkill },
    { "GetTradeskillRepeatCount", &Script_GetTradeskillRepeatCount },
    { "DoTradeSkill", &Script_DoTradeSkill },
    { "GetTradeSkillRecipeLink", &Script_GetTradeSkillRecipeLink },
    { "StopTradeSkillRepeat", &Script_StopTradeSkillRepeat },
    { "GetTradeSkillListLink", &Script_GetTradeSkillListLink },
    { "IsTradeSkillLinked", &Script_IsTradeSkillLinked },
};

// VERIFY: c517a484 auto
// OFFSET: 0x005DA3A0
void TradeSkillRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
