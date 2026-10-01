#include "PetitionInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/CGPetition.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: b24a9953 manual
// OFFSET: 0x005CEAC0
static int32_t Script_GetNumPetitionNames(lua_State* L) {
    lua_pushnumber(L, CGPetitionInfo::m_numPetitionNames);
    return 1;
}

// VERIFY: 7299a8b6 manual
// OFFSET: 0x005CF140
static int32_t Script_SignPetition(lua_State* L) {
    uint8_t slot = 1;

    if (lua_isnumber(L, 1)) {
        slot = static_cast<uint8_t>(lua_tonumber(L, 1));
    }

    WOWGUID petition = CGPetitionInfo::m_petitionGUID;

    if (petition) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_PETITION_SIGN));
        msg.Put(petition.m_guid);
        msg.Put(slot);
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGPetitionInfo::m_signed = 1;
    }

    return 0;
}

// VERIFY: 5cff31d5 manual
// OFFSET: 0x005CF040
static int32_t Script_CanSignPetition(lua_State* L) {
    CGPetition* petition = CGPetitionInfo::m_petition;
    int32_t canSign = 1;

    if (petition) {
        if (petition->m_staticType == 0 || petition->m_staticType == 1) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if ((!petition->m_staticType && player && player->m_player->m_playerGuildid) || static_cast<int32_t>(CGPetitionInfo::m_numPetitionNames) >= static_cast<int32_t>(petition->m_maxSignatures)) {
                canSign = 0;
            }
        }

        if (petition->m_ownerGuid == ClntObjMgrGetActivePlayer() || !canSign) {
            lua_pushnil(L);
            return 1;
        }
    }

    for (uint32_t i = 0; i < CGPetitionInfo::m_numPetitionNames; i++) {
        PetitionSignerInfo* signer = i < CGPetitionInfo::m_numPetitionNames ? &CGPetitionInfo::m_signers.m_data[i] : nullptr;

        if (signer && signer->m_guid == ClntObjMgrGetActivePlayer()) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// STATIC: 0x00AD0A60
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetition", &Script_ClosePetition },
    { "GetPetitionInfo", &Script_GetPetitionInfo },
    { "GetNumPetitionNames", &Script_GetNumPetitionNames },
    { "GetPetitionNameInfo", &Script_GetPetitionNameInfo },
    { "CanSignPetition", &Script_CanSignPetition },
    { "SignPetition", &Script_SignPetition },
    { "OfferPetition", &Script_OfferPetition },
    { "RenamePetition", &Script_RenamePetition },
};

// VERIFY: 6bda2f35 auto
// OFFSET: 0x005CEB00
void PetitionInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
