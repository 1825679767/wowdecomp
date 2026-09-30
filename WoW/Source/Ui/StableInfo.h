#ifndef WOW_SOURCE_UI_STABLEINFO_H
#define WOW_SOURCE_UI_STABLEINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGStableInfo {
    public:
    static WOWGUID m_stableMaster;
    static void CloseStables(bool force);
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_ClosePetStables(lua_State* L);
int32_t Script_StablePet(lua_State* L);
int32_t Script_UnstablePet(lua_State* L);
int32_t Script_BuyStableSlot(lua_State* L);
int32_t Script_GetNumStablePets(lua_State* L);
int32_t Script_GetNumStableSlots(lua_State* L);
int32_t Script_GetStablePetInfo(lua_State* L);
int32_t Script_GetNextStableSlotCost(lua_State* L);
int32_t Script_ClickStablePet(lua_State* L);
int32_t Script_PickupStablePet(lua_State* L);
int32_t Script_GetSelectedStablePet(lua_State* L);
int32_t Script_SetPetStablePaperdoll(lua_State* L);
int32_t Script_GetStablePetFoodTypes(lua_State* L);
int32_t Script_IsAtStableMaster(lua_State* L);
void StableInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_MSG_LIST_STABLED_PETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_STABLE_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
