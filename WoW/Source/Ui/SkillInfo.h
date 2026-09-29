#ifndef WOW_SOURCE_UI_SKILLINFO_H
#define WOW_SOURCE_UI_SKILLINFO_H

#include <cstdint>

struct lua_State;

class CGSkillInfo {
    public:
    static void InitializeGame();
};

int32_t Script_GetNumSkillLines(lua_State* L);
int32_t Script_GetSkillLineInfo(lua_State* L);
int32_t Script_AbandonSkill(lua_State* L);
int32_t Script_CollapseSkillHeader(lua_State* L);
int32_t Script_ExpandSkillHeader(lua_State* L);
int32_t Script_AddSkillUp(lua_State* L);
int32_t Script_RemoveSkillUp(lua_State* L);
int32_t Script_GetAdjustedSkillPoints(lua_State* L);
int32_t Script_AcceptSkillUps(lua_State* L);
int32_t Script_CancelSkillUps(lua_State* L);
int32_t Script_BuySkillTier(lua_State* L);
int32_t Script_SetSelectedSkill(lua_State* L);
int32_t Script_GetSelectedSkill(lua_State* L);
void SkillInfoRegisterScriptFunctions();

#endif
