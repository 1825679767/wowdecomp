#ifndef WOW_SOURCE_UI_STABLEINFO_H
#define WOW_SOURCE_UI_STABLEINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct StablePetInfo {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ char m_name[80];
    /* 005C */ uint8_t unk_005C;
    /* 005D */ uint8_t unk_005D[3];
};

class CGStableInfo {
    public:
    static StablePetInfo m_pets[6];
    static WOWGUID m_stableMaster;
    static int32_t unk_C0F6C8;
    static int32_t unk_C0F6CC;
    static int32_t unk_C0F6D0;

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
