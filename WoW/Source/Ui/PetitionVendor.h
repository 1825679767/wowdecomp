#ifndef WOW_SOURCE_UI_PETITIONVENDOR_H
#define WOW_SOURCE_UI_PETITIONVENDOR_H

#include <cstdint>

struct lua_State;

int32_t Script_ClosePetitionVendor(lua_State* L);
int32_t Script_GetNumPetitionItems(lua_State* L);
int32_t Script_GetPetitionItemInfo(lua_State* L);
int32_t Script_BuyPetition(lua_State* L);
int32_t Script_ClickPetitionButton(lua_State* L);
int32_t Script_TurnInPetition(lua_State* L);
int32_t Script_TurnInArenaPetition(lua_State* L);
int32_t Script_HasFilledPetition(lua_State* L);
void PetitionVendorRegisterScriptFunctions();

class CGPetitionVendorInfo {
    public:
    static void EnterWorld();
};

#endif
