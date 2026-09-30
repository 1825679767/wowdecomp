#ifndef WOW_SOURCE_UI_PETITIONVENDOR_H
#define WOW_SOURCE_UI_PETITIONVENDOR_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

int32_t Script_ClosePetitionVendor(lua_State* L);
int32_t Script_GetPetitionItemInfo(lua_State* L);
int32_t Script_BuyPetition(lua_State* L);
int32_t Script_ClickPetitionButton(lua_State* L);
int32_t Script_TurnInArenaPetition(lua_State* L);
int32_t Script_HasFilledPetition(lua_State* L);
void PetitionVendorRegisterScriptFunctions();
void PetitionVendorClose();

class CGPetitionVendorInfo {
    public:
    static WOWGUID m_vendor;
    static WOWGUID m_petitionItem;
    static void EnterWorld();
};

#endif
