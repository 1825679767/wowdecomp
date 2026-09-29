#ifndef WOW_SOURCE_UI_CLASSTRAINERFRAME_H
#define WOW_SOURCE_UI_CLASSTRAINERFRAME_H

#include <cstddef>
#include <cstdint>

struct lua_State;

class CGClassTrainer {
    public:
    static void EnterWorld();
    static void RefreshList();
    static void InitializeGame();
};

int32_t Script_OpenTrainer(lua_State* L);
int32_t Script_CloseTrainer(lua_State* L);
int32_t Script_GetNumTrainerServices(lua_State* L);
int32_t Script_GetTrainerServiceInfo(lua_State* L);
int32_t Script_SelectTrainerService(lua_State* L);
int32_t Script_IsTradeskillTrainer(lua_State* L);
int32_t Script_GetTrainerSelectionIndex(lua_State* L);
int32_t Script_GetTrainerGreetingText(lua_State* L);
int32_t Script_GetTrainerServiceIcon(lua_State* L);
int32_t Script_GetTrainerServiceSkillLine(lua_State* L);
int32_t Script_GetTrainerServiceCost(lua_State* L);
int32_t Script_GetTrainerServiceLevelReq(lua_State* L);
int32_t Script_GetTrainerServiceSkillReq(lua_State* L);
int32_t Script_GetTrainerServiceNumAbilityReq(lua_State* L);
int32_t Script_GetTrainerServiceAbilityReq(lua_State* L);
int32_t Script_GetTrainerServiceStepReq(lua_State* L);
int32_t Script_GetTrainerServiceDescription(lua_State* L);
int32_t Script_IsTrainerServiceSkillStep(lua_State* L);
int32_t Script_GetTrainerServiceStepIncrease(lua_State* L);
int32_t Script_BuyTrainerService(lua_State* L);
int32_t Script_SetTrainerServiceTypeFilter(lua_State* L);
int32_t Script_SetTrainerSkillLineFilter(lua_State* L);
int32_t Script_GetTrainerServiceTypeFilter(lua_State* L);
int32_t Script_GetTrainerSkillLineFilter(lua_State* L);
int32_t Script_GetTrainerSkillLines(lua_State* L);
int32_t Script_CollapseTrainerSkillLine(lua_State* L);
int32_t Script_ExpandTrainerSkillLine(lua_State* L);
int32_t Script_GetTrainerServiceItemLink(lua_State* L);
void ClassTrainerRegisterScriptFunctions();

#endif
