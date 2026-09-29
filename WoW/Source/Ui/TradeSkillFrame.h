#ifndef WOW_SOURCE_UI_TRADESKILLFRAME_H
#define WOW_SOURCE_UI_TRADESKILLFRAME_H

#include <cstdint>

struct lua_State;

class CGTradeSkillInfo {
    public:
    static void RefreshList(int32_t a1);
    static void CommitPendingRecast(int32_t spellId);
    static void DoRecast(int32_t spellId);
};

int32_t Script_CloseTradeSkill(lua_State* L);
int32_t Script_GetNumTradeSkills(lua_State* L);
int32_t Script_GetTradeSkillInfo(lua_State* L);
int32_t Script_SelectTradeSkill(lua_State* L);
int32_t Script_GetTradeSkillSelectionIndex(lua_State* L);
int32_t Script_GetTradeSkillCooldown(lua_State* L);
int32_t Script_GetTradeSkillIcon(lua_State* L);
int32_t Script_GetTradeSkillNumMade(lua_State* L);
int32_t Script_GetTradeSkillLine(lua_State* L);
int32_t Script_GetTradeSkillItemLink(lua_State* L);
int32_t Script_SetTradeSkillItemNameFilter(lua_State* L);
int32_t Script_GetTradeSkillItemNameFilter(lua_State* L);
int32_t Script_SetTradeSkillItemLevelFilter(lua_State* L);
int32_t Script_GetTradeSkillItemLevelFilter(lua_State* L);
int32_t Script_GetTradeSkillNumReagents(lua_State* L);
int32_t Script_GetTradeSkillReagentInfo(lua_State* L);
int32_t Script_GetTradeSkillReagentItemLink(lua_State* L);
int32_t Script_GetTradeSkillTools(lua_State* L);
int32_t Script_GetTradeSkillDescription(lua_State* L);
int32_t Script_GetTradeSkillSubClasses(lua_State* L);
int32_t Script_GetTradeSkillInvSlots(lua_State* L);
int32_t Script_SetTradeSkillSubClassFilter(lua_State* L);
int32_t Script_GetTradeSkillSubClassFilter(lua_State* L);
int32_t Script_SetTradeSkillInvSlotFilter(lua_State* L);
int32_t Script_GetTradeSkillInvSlotFilter(lua_State* L);
int32_t Script_TradeSkillOnlyShowMakeable(lua_State* L);
int32_t Script_TradeSkillOnlyShowSkillUps(lua_State* L);
int32_t Script_CollapseTradeSkillSubClass(lua_State* L);
int32_t Script_ExpandTradeSkillSubClass(lua_State* L);
int32_t Script_GetFirstTradeSkill(lua_State* L);
int32_t Script_GetTradeskillRepeatCount(lua_State* L);
int32_t Script_DoTradeSkill(lua_State* L);
int32_t Script_GetTradeSkillRecipeLink(lua_State* L);
int32_t Script_StopTradeSkillRepeat(lua_State* L);
int32_t Script_GetTradeSkillListLink(lua_State* L);
int32_t Script_IsTradeSkillLinked(lua_State* L);
void TradeSkillRegisterScriptFunctions();

#endif
