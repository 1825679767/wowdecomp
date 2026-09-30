#include "CGBankInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/BankBagSlotPricesRec.h"

// VERIFY: 5ee8d6b2 manual
// OFFSET: 0x0057B940
static int32_t Script_BankButtonIDToInvSlotID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BankButtonIDToInvSlotID(buttonID)");
        return 0;
    }

    int32_t button = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t slot;

    if (lua_isnumber(L, 2)) {
        slot = button + 63;
    } else {
        slot = button + 39;
    }

    lua_pushnumber(L, slot + 1);
    return 1;
}

// VERIFY: 3ad30a07 manual
// OFFSET: 0x0057BAB0
static int32_t Script_GetNumBankSlots(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t slots = player ? player->m_player->m_playerBytes2[2] : 0;
    lua_pushnumber(L, slots);

    if (slots < 7) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 2;
}

// VERIFY: 93072092 manual
// OFFSET: 0x0057BB30
static int32_t Script_GetBankSlotCost(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const BankBagSlotPricesRec* rec;
    int32_t cost;

    if (player && (rec = g_bankBagSlotPricesDB.GetRecord(player->m_player->m_playerBytes2[2] + 1))) {
        cost = rec->m_cost;
    } else {
        cost = 0;
    }

    lua_pushnumber(L, cost);
    return 1;
}

// STATIC: 0x00ACEB7C
static FrameScript_Method s_ScriptFunctions[] = {
    { "BankButtonIDToInvSlotID", &Script_BankButtonIDToInvSlotID },
    { "GetNumBankSlots", &Script_GetNumBankSlots },
    { "GetBankSlotCost", &Script_GetBankSlotCost },
    { "PurchaseSlot", &Script_PurchaseSlot },
    { "CloseBankFrame", &Script_CloseBankFrame },
};

// VERIFY: 221cb74e auto
// OFFSET: 0x0057B9E0
void BankRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
