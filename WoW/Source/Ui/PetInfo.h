#ifndef WOW_SOURCE_UI_PETINFO_H
#define WOW_SOURCE_UI_PETINFO_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGPetInfo {
    public:
    static void InitializeGame();
    static void FindPossessedSpell();
    static WOWGUID GetPet(uint32_t index);
    static void UpdateCooldowns();
    static WOWGUID GetTarget();
    static void HideGrid();
    static uint8_t GetComboPoints(WOWGUID target);
    static bool PetActionsUsable(bool a1);
    static uint8_t m_comboPoints;
    static WOWGUID m_comboTarget;
    static void PetClearTarget();
    static void PetAttackTarget(const WOWGUID& target, int32_t a2);
    static int32_t m_attackingTarget;
    static uint32_t m_actions[10];
    static uint32_t m_petMode;
    static uint32_t GetPetMode();
    static void SetPet(WOWGUID pet, uint32_t a2, int32_t a3);
    static void SendPetAction(const uint32_t& action, const WOWGUID& target, int32_t a3);
    static const char* GetModeToken(uint32_t mode);
    static const char* GetOrdersToken(uint32_t order);
};

int32_t Script_PetHasActionBar(lua_State* L);
int32_t Script_GetPetActionInfo(lua_State* L);
int32_t Script_GetPetActionCooldown(lua_State* L);
int32_t Script_GetPetActionsUsable(lua_State* L);
int32_t Script_GetPetActionSlotUsable(lua_State* L);
int32_t Script_IsPetAttackActive(lua_State* L);
int32_t Script_PickupPetAction(lua_State* L);
int32_t Script_TogglePetAutocast(lua_State* L);
int32_t Script_CastPetAction(lua_State* L);
int32_t Script_PetPassiveMode(lua_State* L);
int32_t Script_PetDefensiveMode(lua_State* L);
int32_t Script_PetAggressiveMode(lua_State* L);
int32_t Script_PetWait(lua_State* L);
int32_t Script_PetFollow(lua_State* L);
int32_t Script_PetAttack(lua_State* L);
int32_t Script_PetStopAttack(lua_State* L);
int32_t Script_PetAbandon(lua_State* L);
int32_t Script_PetDismiss(lua_State* L);
int32_t Script_PetRename(lua_State* L);
int32_t Script_PetCanBeAbandoned(lua_State* L);
int32_t Script_PetCanBeDismissed(lua_State* L);
int32_t Script_PetCanBeRenamed(lua_State* L);
int32_t Script_GetPetTimeRemaining(lua_State* L);
int32_t Script_HasPetUI(lua_State* L);
int32_t Script_GetPetExperience(lua_State* L);
int32_t Script_GetPetHappiness(lua_State* L);
int32_t Script_GetPetFoodTypes(lua_State* L);
int32_t Script_GetPetIcon(lua_State* L);
int32_t Script_GetPetTalentTree(lua_State* L);
int32_t Script_GetPossessInfo(lua_State* L);
int32_t Script_IsPetAttackAction(lua_State* L);
void PetInfoRegisterScriptFunctions();

#endif
