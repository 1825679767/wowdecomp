#ifndef WOW_SOURCE_UI_ACTIONBARFRAME_H
#define WOW_SOURCE_UI_ACTIONBARFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGItem_C;

class CGActionBar {
    public:
    static uint32_t m_tempPageActiveFlags;
    static int32_t m_bonusBarOffset;
    static void UpdateOverrideSpellBar();
    static void EnterWorld();
    static int32_t m_slotNoMana[144];
    static int32_t m_slotUsable[144];
    static int32_t m_slotItemCount[144];
    static uint32_t m_slotActions[144];

    static int32_t IsMacro(uint32_t slot);
    static void OnAuraChanged(int32_t spellId);
    static uint32_t GetMacroID(uint32_t slot);
    static int32_t IsItem(uint32_t slot);
    static int32_t IsPetAction(uint32_t slot);
    static int32_t IsEquipmentSet(uint32_t slot);
    static int32_t GetEquipmentSetID(uint32_t slot);
    static int32_t HasMacro(uint32_t slot);
    static int32_t GetItem(uint32_t slot);
    static int32_t GetMacroItem(uint32_t slot);
    static int32_t GetMacroSpell(uint32_t slot, int32_t& petSpell);
    static int32_t GetItemSpell(uint32_t slot);
    static int32_t GetPetActionSpell(uint32_t slot);
    static CGItem_C* FindUseEnchantItemPtr(int32_t itemId);
    static int32_t GetSpell(uint32_t slot, int32_t& petSpell);
    static int32_t IsAttackAction(uint32_t slot);
    static int32_t IsRangedAttackAction(uint32_t slot);
    static int32_t UpdateUsableAction(uint32_t slot, int32_t& noMana);
    static void SetAction(int32_t slot, uint32_t action, int32_t sendToServer, int32_t silent);
    static void UpdateBonusBar();
    static void UpdateShapeShiftBar();
    static void InitializeGame();
    static void UpdateAttackAction();
    static void UpdateItem(int32_t itemId);
    static void UpdateRangedAttackAction();
    static void HideGrid();
    static void UpdateUsable();
    static void RemoveAction(uint32_t slot);
    static void ReplaceSpell(int32_t oldSpell, int32_t newSpell);
    static int32_t HasAction(uint32_t slot);
    static void PutActionInSlot(int32_t slot);
    static void UpdateMultiCastBar();
    static uint32_t GetMultiCastSlotMaskForTotemCategory(int32_t category);
    static bool IsEquipmentSetValid(int32_t setId);
    static int32_t IsSpell(uint32_t slot);
    static void ShowGrid();
    static void UpdateSelection();
    static const char* GetAttackTexture();
    static const char* GetRangedTexture();
    static int32_t CalculateItemCount(int32_t itemId);
    static void GetCooldown(int32_t slot, uint32_t& start, uint32_t& duration, uint32_t& enable);
    static const char* GetTexture(int32_t slot);
    static int32_t IsCurrentAction(int32_t slot);
    static void SlotChanged(int32_t slot, int32_t sendToServer, int32_t silent);
    static void UseAction(int32_t slot, const WOWGUID& target, const char* button);
    static void UpdateMacro(uint32_t macroId);
};

void ItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

int32_t Script_GetActionInfo(lua_State* L);
int32_t Script_GetActionTexture(lua_State* L);
int32_t Script_GetActionCount(lua_State* L);
int32_t Script_GetActionCooldown(lua_State* L);
int32_t Script_GetActionAutocast(lua_State* L);
int32_t Script_GetActionText(lua_State* L);
int32_t Script_HasAction(lua_State* L);
int32_t Script_UseAction(lua_State* L);
int32_t Script_PickupAction(lua_State* L);
int32_t Script_PlaceAction(lua_State* L);
int32_t Script_IsAttackAction(lua_State* L);
int32_t Script_IsCurrentAction(lua_State* L);
int32_t Script_IsAutoRepeatAction(lua_State* L);
int32_t Script_IsUsableAction(lua_State* L);
int32_t Script_IsConsumableAction(lua_State* L);
int32_t Script_IsStackableAction(lua_State* L);
int32_t Script_IsEquippedAction(lua_State* L);
int32_t Script_ActionHasRange(lua_State* L);
int32_t Script_IsActionInRange(lua_State* L);
int32_t Script_GetBonusBarOffset(lua_State* L);
int32_t Script_GetMultiCastBarOffset(lua_State* L);
int32_t Script_ChangeActionBarPage(lua_State* L);
int32_t Script_GetActionBarPage(lua_State* L);
int32_t Script_GetActionBarToggles(lua_State* L);
int32_t Script_SetActionBarToggles(lua_State* L);
int32_t Script_IsPossessBarVisible(lua_State* L);
int32_t Script_GetMultiCastTotemSpells(lua_State* L);
int32_t Script_SetMultiCastSpell(lua_State* L);
void ActionBarRegisterScriptFunctions();

#endif
