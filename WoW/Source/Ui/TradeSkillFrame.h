#ifndef WOW_SOURCE_UI_TRADESKILLFRAME_H
#define WOW_SOURCE_UI_TRADESKILLFRAME_H

#include <cstdint>

struct lua_State;

class CGTradeSkillInfo {
    public:
    static int32_t m_filteredSkills;
    static int32_t m_spellID;
    static void* m_linkData;
    static int32_t m_skillLineID;
    static char* m_itemNameFilter;
    static int32_t m_itemMinLevelFilter;
    static int32_t m_itemMaxLevelFilter;
    static int32_t m_recastSpellID;
    static int32_t m_pendingRecastSpellID;
    static int32_t m_selectedSkill;
    static uint32_t m_recastTimes;
    static uint32_t m_pendingRecastTimes;
    static void RefreshList(int32_t a1);
    static void SetLink(const char* link);
    static void CommitPendingRecast(int32_t spellId);
    static void DoRecast(int32_t spellId);
};

int32_t Script_GetTradeSkillInfo(lua_State* L);
int32_t Script_SelectTradeSkill(lua_State* L);
int32_t Script_GetTradeSkillSelectionIndex(lua_State* L);
int32_t Script_GetTradeSkillCooldown(lua_State* L);
int32_t Script_GetTradeSkillIcon(lua_State* L);
int32_t Script_GetTradeSkillNumMade(lua_State* L);
int32_t Script_GetTradeSkillLine(lua_State* L);
int32_t Script_GetTradeSkillItemLink(lua_State* L);
int32_t Script_SetTradeSkillItemNameFilter(lua_State* L);
int32_t Script_SetTradeSkillItemLevelFilter(lua_State* L);
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
int32_t Script_DoTradeSkill(lua_State* L);
int32_t Script_GetTradeSkillRecipeLink(lua_State* L);
int32_t Script_GetTradeSkillListLink(lua_State* L);
int32_t Script_IsTradeSkillLinked(lua_State* L);
void TradeSkillRegisterScriptFunctions();

#endif
