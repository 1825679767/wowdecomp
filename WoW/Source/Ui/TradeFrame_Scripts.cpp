#include "TradeFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Object/ObjectClient/Trade_C.h"

// VERIFY: 35618cea auto
// OFFSET: 0x00586730
static int32_t Script_CancelTradeAccept(lua_State* L) {
    if (CGGameUI::CanPerformAction(7) && CGTradeInfo::m_playerAccepted) {
        CGTradeInfo::m_playerAccepted = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_ACCEPT_UPDATE, "%d%d", 0, CGTradeInfo::m_targetAccepted);
        Trade_C_UnacceptTrade();
    }

    return 0;
}

// VERIFY: 2d9f2b3c manual
// OFFSET: 0x00586780
static int32_t Script_GetPlayerTradeMoney(lua_State* L) {
    uint32_t money = CGTradeInfo::m_tradingPlayer ? CGTradeInfo::GetPlayerTradeMoney() : 0;
    lua_pushnumber(L, money);
    return 1;
}

// VERIFY: 89405660 manual
// OFFSET: 0x005867D0
static int32_t Script_GetTargetTradeMoney(lua_State* L) {
    lua_pushnumber(L, CGTradeInfo::GetTargetTradeMoney());
    return 1;
}

// STATIC: 0x00ACEDB0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTrade", &Script_CloseTrade },
    { "ClickTradeButton", &Script_ClickTradeButton },
    { "ClickTargetTradeButton", &Script_ClickTargetTradeButton },
    { "GetTradeTargetItemInfo", &Script_GetTradeTargetItemInfo },
    { "GetTradeTargetItemLink", &Script_GetTradeTargetItemLink },
    { "GetTradePlayerItemInfo", &Script_GetTradePlayerItemInfo },
    { "GetTradePlayerItemLink", &Script_GetTradePlayerItemLink },
    { "AcceptTrade", &Script_AcceptTrade },
    { "CancelTradeAccept", &Script_CancelTradeAccept },
    { "GetPlayerTradeMoney", &Script_GetPlayerTradeMoney },
    { "GetTargetTradeMoney", &Script_GetTargetTradeMoney },
    { "PickupTradeMoney", &Script_PickupTradeMoney },
    { "AddTradeMoney", &Script_AddTradeMoney },
    { "SetTradeMoney", &Script_SetTradeMoney },
};

// VERIFY: d36bca96 auto
// OFFSET: 0x005868E0
void TradeInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
