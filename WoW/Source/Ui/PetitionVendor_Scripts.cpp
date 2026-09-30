#include "PetitionVendor.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"

// STATIC: 0x00C0F7D0
static uint32_t s_numPetitionItems;

// VERIFY: 7b8583d7 manual
// OFFSET: 0x005A1F70
static int32_t Script_GetNumPetitionItems(lua_State* L) {
    lua_pushnumber(L, s_numPetitionItems);
    return 1;
}

// VERIFY: b1f3e113 manual
// OFFSET: 0x005A2600
static int32_t Script_TurnInPetition(lua_State* L) {
    if (!CGPetitionVendorInfo::m_vendor) {
        return 0;
    }

    WOWGUID item = CGPetitionVendorInfo::m_petitionItem;

    if (item) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_TURN_IN_PETITION));
        msg.Put(item.m_guid);
        msg.Put(static_cast<uint32_t>(-1));
        msg.Put(static_cast<uint32_t>(-1));
        msg.Put(static_cast<uint32_t>(-1));
        msg.Put(static_cast<uint32_t>(-1));
        msg.Put(static_cast<uint32_t>(-1));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// STATIC: 0x00ACF7F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetitionVendor", &Script_ClosePetitionVendor },
    { "GetNumPetitionItems", &Script_GetNumPetitionItems },
    { "GetPetitionItemInfo", &Script_GetPetitionItemInfo },
    { "BuyPetition", &Script_BuyPetition },
    { "ClickPetitionButton", &Script_ClickPetitionButton },
    { "TurnInPetition", &Script_TurnInPetition },
    { "TurnInArenaPetition", &Script_TurnInArenaPetition },
    { "HasFilledPetition", &Script_HasFilledPetition },
};

// VERIFY: b54a5949 auto
// OFFSET: 0x005A20F0
void PetitionVendorRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
