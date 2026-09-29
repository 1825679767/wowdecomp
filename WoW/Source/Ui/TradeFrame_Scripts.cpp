#include "TradeFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
