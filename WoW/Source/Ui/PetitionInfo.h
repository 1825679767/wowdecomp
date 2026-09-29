#ifndef WOW_SOURCE_UI_PETITIONINFO_H
#define WOW_SOURCE_UI_PETITIONINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"

struct lua_State;

class CGPetitionInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_ClosePetition(lua_State* L);
int32_t Script_GetPetitionInfo(lua_State* L);
int32_t Script_GetNumPetitionNames(lua_State* L);
int32_t Script_GetPetitionNameInfo(lua_State* L);
int32_t Script_CanSignPetition(lua_State* L);
int32_t Script_SignPetition(lua_State* L);
int32_t Script_OfferPetition(lua_State* L);
int32_t Script_RenamePetition(lua_State* L);
void PetitionInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_OFFER_PETITION_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
