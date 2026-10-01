#ifndef WOW_SOURCE_UI_PETINFO_H
#define WOW_SOURCE_UI_PETINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

class CGPetInfo {
    public:
    static void InitializeGame();
    static void FindPossessedSpell();
    static WOWGUID GetPet(uint32_t index);
    static uint32_t* FindSpellByID(int32_t spellId);
    static int32_t IsPetBarUsed();
    static void UpdateCooldowns();
    static WOWGUID GetTarget();
    static void HideGrid();
    static uint8_t GetComboPoints(WOWGUID target);
    static bool PetActionsUsable(bool a1);
    static uint8_t m_comboPoints;
    static int32_t m_possessSpell;
    static WOWGUID m_comboTarget;
    static void PetClearTarget();
    static void PetDismiss();
    static void PetAttackTarget(const WOWGUID& target, int32_t a2);
    static int32_t m_attackingTarget;
    static uint32_t m_actions[10];
    static uint32_t m_petMode;
    static uint32_t m_expirationTime;
    static WOWGUID m_pet;
    static uint32_t m_petBarState;
    static void SetPetCount(uint32_t count);
    static int32_t IsAttackAction(uint32_t index);
    static uint32_t GetPetMode();
    static void SetPet(WOWGUID pet, uint32_t expiration, int32_t family);
    static void SetComboPoints(WOWGUID target, uint8_t points);
    static void SetIsAttackingTarget(int32_t attacking);
    static void UpdatePetTempBar();
    static void SendPetAction(const uint32_t& action, const WOWGUID& target, int32_t a3);
    static uint32_t* GetPetSpellAction(int32_t spellId);
    static void UpdatePetSpellAction(const uint32_t* action);
    static void SetPossessedSpell(int32_t spellId);
    static void SetAction(uint32_t index, const uint32_t& action, int32_t a3);
    static void SetSpellAutocast(int32_t spellId, int32_t enable);
    static void SetPetMode(uint32_t mode);
    static void SetPetOrders(uint32_t orders);
    static const char* GetModeToken(uint32_t mode);
    static const char* GetOrdersToken(uint32_t order);
};

int32_t Script_GetPetActionsUsable(lua_State* L);
int32_t Script_GetPetActionSlotUsable(lua_State* L);
int32_t Script_PickupPetAction(lua_State* L);
int32_t Script_TogglePetAutocast(lua_State* L);
int32_t Script_CastPetAction(lua_State* L);
int32_t Script_PetPassiveMode(lua_State* L);
int32_t Script_PetDefensiveMode(lua_State* L);
int32_t Script_PetAggressiveMode(lua_State* L);
int32_t Script_PetWait(lua_State* L);
int32_t Script_PetFollow(lua_State* L);
int32_t Script_PetAttack(lua_State* L);
int32_t Script_PetAbandon(lua_State* L);
int32_t Script_PetDismiss(lua_State* L);
int32_t Script_PetRename(lua_State* L);
int32_t Script_GetPetHappiness(lua_State* L);
int32_t Script_GetPetFoodTypes(lua_State* L);
int32_t Script_GetPetTalentTree(lua_State* L);
void PetInfoRegisterScriptFunctions();
int32_t Packet_SMSG_PET_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_7(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_MODE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_ACTION_FEEDBACK(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_BROKEN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_RENAMEABLE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_UPDATE_COMBO_POINTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_GUIDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

extern TSGrowableArray<WOWGUID> s_pets;

#endif
