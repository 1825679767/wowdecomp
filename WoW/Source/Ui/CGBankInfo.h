#ifndef WOW_SOURCE_UI_CGBANKINFO_H
#define WOW_SOURCE_UI_CGBANKINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/Object_C.h"

struct lua_State;

class CGBankInfo {
    public:
    static void EnterWorld();
    static WOWGUID m_banker;
};

int32_t Script_BankButtonIDToInvSlotID(lua_State* L);
int32_t Script_GetNumBankSlots(lua_State* L);
int32_t Script_GetBankSlotCost(lua_State* L);
int32_t Script_PurchaseSlot(lua_State* L);
int32_t Script_CloseBankFrame(lua_State* L);
void BankRegisterScriptFunctions();

#endif
