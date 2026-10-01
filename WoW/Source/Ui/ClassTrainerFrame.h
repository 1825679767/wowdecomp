#ifndef WOW_SOURCE_UI_CLASSTRAINERFRAME_H
#define WOW_SOURCE_UI_CLASSTRAINERFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct TrainerServiceInfo {
    /* 0000 */ int32_t m_spellID;
    /* 0004 */ int32_t m_skillLine;
    /* 0008 */ int32_t m_moneyCost;
    /* 000C */ int32_t m_talentCost;
    /* 0010 */ int32_t m_professionCost;
    /* 0014 */ uint8_t m_levelReq;
    /* 0015 */ uint8_t unk_0015[3];
    /* 0018 */ int32_t m_skillReq;
    /* 001C */ int32_t m_skillRankReq;
    /* 0020 */ int32_t m_stepSpellID;
    /* 0024 */ int32_t m_abilityReq[3];
    /* 0030 */ uint8_t m_state;
    /* 0031 */ uint8_t unk_0031[3];
    /* 0034 */ int32_t m_visible;
};

struct TrainerSkillLineInfo {
    /* 0000 */ int32_t m_skillLine;
    /* 0004 */ uint32_t m_stateCounts[6];
    /* 001C */ int32_t m_visible;
    /* 0020 */ int32_t m_collapsed;
    /* 0024 */ int32_t unk_0024;
};

class CGClassTrainer {
    public:
    static void ShutdownGame();
    static WOWGUID m_trainer;
    static char m_greetingText[2048];
    static int32_t m_trainerType;
    static uint32_t m_filteredServices;
    static int32_t m_currentSelection;
    static uint32_t m_numServices;
    static int32_t m_numSkillLines;
    static uint32_t m_skillLineFilter;
    static uint32_t m_expandedSkillLines;
    static TSGrowableArray<TrainerServiceInfo*> m_services;
    static TSGrowableArray<TrainerSkillLineInfo*> m_skillLines;

    static void SetTrainer(const WOWGUID& trainer, int32_t type);
    static void EnterWorld();
    static void FilterAndSortServices();
    static void RefreshList();
    static void InitializeGame();
};

int32_t Script_OpenTrainer(lua_State* L);
int32_t Script_CloseTrainer(lua_State* L);
int32_t Script_GetTrainerServiceInfo(lua_State* L);
int32_t Script_GetTrainerSelectionIndex(lua_State* L);
int32_t Script_GetTrainerServiceIcon(lua_State* L);
int32_t Script_GetTrainerServiceSkillReq(lua_State* L);
int32_t Script_GetTrainerServiceAbilityReq(lua_State* L);
int32_t Script_GetTrainerServiceStepReq(lua_State* L);
int32_t Script_GetTrainerServiceDescription(lua_State* L);
int32_t Script_GetTrainerServiceStepIncrease(lua_State* L);
int32_t Script_BuyTrainerService(lua_State* L);
int32_t Script_SetTrainerServiceTypeFilter(lua_State* L);
int32_t Script_SetTrainerSkillLineFilter(lua_State* L);
int32_t Script_GetTrainerServiceTypeFilter(lua_State* L);
int32_t Script_CollapseTrainerSkillLine(lua_State* L);
int32_t Script_ExpandTrainerSkillLine(lua_State* L);
int32_t Script_GetTrainerServiceItemLink(lua_State* L);
void ClassTrainerRegisterScriptFunctions();

#endif
