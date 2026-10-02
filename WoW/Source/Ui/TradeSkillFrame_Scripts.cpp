#include "TradeSkillFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/Types.h"

// VERIFY: 580553ac auto
// OFFSET: 0x005DA120
static int32_t Script_GetNumTradeSkills(lua_State* L) {
    lua_pushnumber(L, CGTradeSkillInfo::m_filteredSkills);
    return 1;
}

// VERIFY: e6343d59 auto
// OFFSET: 0x005DA150
static int32_t Script_GetTradeSkillItemNameFilter(lua_State* L) {
    lua_pushstring(L, CGTradeSkillInfo::m_itemNameFilter);
    return 1;
}

// VERIFY: 992ebdfa manual
// OFFSET: 0x005DA2E0
static int32_t Script_StopTradeSkillRepeat(lua_State* L) {
    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }

    return 0;
}

// VERIFY: 1299596b auto
// OFFSET: 0x005DA170
static int32_t Script_GetTradeSkillItemLevelFilter(lua_State* L) {
    int32_t maxLevel = CGTradeSkillInfo::m_itemMaxLevelFilter;
    lua_pushnumber(L, CGTradeSkillInfo::m_itemMinLevelFilter);
    lua_pushnumber(L, maxLevel);
    return 2;
}

// VERIFY: edab51c5 manual
// OFFSET: 0x005DA8B0
static int32_t Script_CloseTradeSkill(lua_State* L) {
    CGTradeSkillInfo::m_spellID = 0;
    CGTradeSkillInfo::m_skillLineID = 0;

    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, nullptr);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_CLOSE, nullptr);
    return 0;
}

// VERIFY: 1d8d0d2f manual
// OFFSET: 0x005DA270
static int32_t Script_GetTradeskillRepeatCount(lua_State* L) {
    uint32_t count;

    if (CGTradeSkillInfo::m_recastSpellID && CGTradeSkillInfo::m_recastSpellID == CGTradeSkillInfo::m_selectedSkill) {
        count = CGTradeSkillInfo::m_recastTimes;
    } else if (CGTradeSkillInfo::m_pendingRecastSpellID && CGTradeSkillInfo::m_pendingRecastSpellID == CGTradeSkillInfo::m_selectedSkill) {
        count = CGTradeSkillInfo::m_pendingRecastTimes;
    } else {
        count = 1;
    }

    if (count <= 1) {
        count = 1;
    }

    lua_pushnumber(L, count);
    return 1;
}

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

// VERIFY: 02805132 auto
// OFFSET: 0x005DA3D0
void TradeSkillUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
