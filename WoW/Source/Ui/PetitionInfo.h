#ifndef WOW_SOURCE_UI_PETITIONINFO_H
#define WOW_SOURCE_UI_PETITIONINFO_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class CGPetition;

struct PetitionSignerInfo {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
};

class CGPetitionInfo {
    public:
    static void ShutdownGame();
    static WOWGUID m_petitionGUID;
    static WOWGUID m_petitionItem;
    static uint32_t unk_C22B30;
    static int32_t m_signed;
    static uint32_t m_numPetitionNames;
    static int32_t m_pendingNameQueries;
    static CGPetition* m_petition;
    static TSGrowableArray<PetitionSignerInfo> m_signers;

    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_ClosePetition(lua_State* L);
int32_t Script_GetPetitionInfo(lua_State* L);
int32_t Script_GetPetitionNameInfo(lua_State* L);
int32_t Script_OfferPetition(lua_State* L);
int32_t Script_RenamePetition(lua_State* L);
void PetitionInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_SMSG_OFFER_PETITION_ERROR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
