#ifndef WOW_SOURCE_UI_TRADESKILLFRAME_H
#define WOW_SOURCE_UI_TRADESKILLFRAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct TradeSkillInfo {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ int32_t m_difficulty;
    /* 0008 */ int32_t m_itemClass;
    /* 000C */ int32_t m_itemSubClass;
    /* 0010 */ uint32_t m_invTypeMask;
    /* 0014 */ int32_t m_itemLevel;
    /* 0018 */ int32_t m_numAvailable;
    /* 001C */ int32_t m_skillLineRank;
    /* 0020 */ int32_t m_visible;
};

struct TradeSkillSubClassInfo {
    /* 0000 */ int32_t m_itemClass;
    /* 0004 */ int32_t m_itemSubClass;
    /* 0008 */ int32_t m_shown;
    /* 000C */ int32_t m_collapsed;
    /* 0010 */ uint32_t m_invTypeMask;
};

class CGTradeSkillInfo {
    public:
    static void Close();
    static void ShutdownGame();
    static WOWGUID m_playerGuid;
    static uint32_t m_itemsPending;
    static uint32_t m_numDisplayEntries;
    static uint32_t m_numSubClasses;
    static int32_t m_showOnlyMakeable;
    static int32_t m_showOnlySkillUps;
    static uint32_t m_invSlots;
    static int32_t m_skillRank;
    static int32_t m_skillMaxRank;
    static int32_t unk_C235F0;
    static TSGrowableArray<TradeSkillInfo*> m_displayList;
    static TSGrowableArray<TradeSkillSubClassInfo*> m_subClassList;
    static uint32_t m_subClassFilter;
    static uint32_t m_invTypeFilter;
    static uint32_t m_collapseFilter;
    static void ClearItemCallbacks();
    static void SetInvTypeFilter(int32_t a1);
    static int32_t GetSubClassIndexFromSkill(uint32_t index);
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
    static void RefreshList(int32_t full);
    static void SetLink(const char* link);
    static void CommitPendingRecast(int32_t spellId);
    static void DoRecast(int32_t spellId);
    static void StopRecast();
    static bool GetTradeSkillListLink(int32_t spellId, char* buffer, uint32_t bufferSize);
    static void SetTradeSkill(int32_t spellId, const WOWGUID& guid, int32_t a3, int32_t a4, int32_t a5);
};

int32_t QSortSubClasses(const void* a, const void* b);
int32_t QSortSkills(const void* a, const void* b);
bool Profession_FilterSpell(int32_t spellId, int32_t minLevel, int32_t maxLevel, const char* name);
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
void UpdateTradeSkillRecast(int32_t spellId);

#endif
