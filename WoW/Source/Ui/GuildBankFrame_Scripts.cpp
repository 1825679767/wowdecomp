#include "GuildBankFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF8D0
static FrameScript_Method s_ScriptFunctions[] = {
    { "QueryGuildBankTab", &Script_QueryGuildBankTab },
    { "SetCurrentGuildBankTab", &Script_SetCurrentGuildBankTab },
    { "GetCurrentGuildBankTab", &Script_GetCurrentGuildBankTab },
    { "GetGuildBankItemInfo", &Script_GetGuildBankItemInfo },
    { "SetGuildBankTabInfo", &Script_SetGuildBankTabInfo },
    { "GetGuildBankItemLink", &Script_GetGuildBankItemLink },
    { "PickupGuildBankItem", &Script_PickupGuildBankItem },
    { "AutoStoreGuildBankItem", &Script_AutoStoreGuildBankItem },
    { "SplitGuildBankItem", &Script_SplitGuildBankItem },
    { "GetNumGuildBankTabs", &Script_GetNumGuildBankTabs },
    { "GetGuildBankTabInfo", &Script_GetGuildBankTabInfo },
    { "GetGuildBankTabCost", &Script_GetGuildBankTabCost },
    { "BuyGuildBankTab", &Script_BuyGuildBankTab },
    { "DepositGuildBankMoney", &Script_DepositGuildBankMoney },
    { "WithdrawGuildBankMoney", &Script_WithdrawGuildBankMoney },
    { "CanWithdrawGuildBankMoney", &Script_CanWithdrawGuildBankMoney },
    { "PickupGuildBankMoney", &Script_PickupGuildBankMoney },
    { "GetGuildBankMoney", &Script_GetGuildBankMoney },
    { "GetGuildBankWithdrawMoney", &Script_GetGuildBankWithdrawMoney },
    { "CloseGuildBankFrame", &Script_CloseGuildBankFrame },
    { "GetGuildTabardFileNames", &Script_GetGuildTabardFileNames },
    { "QueryGuildBankLog", &Script_QueryGuildBankLog },
    { "GetNumGuildBankTransactions", &Script_GetNumGuildBankTransactions },
    { "GetGuildBankTransaction", &Script_GetGuildBankTransaction },
    { "GetNumGuildBankMoneyTransactions", &Script_GetNumGuildBankMoneyTransactions },
    { "GetGuildBankMoneyTransaction", &Script_GetGuildBankMoneyTransaction },
    { "QueryGuildBankText", &Script_QueryGuildBankText },
    { "GetGuildBankText", &Script_GetGuildBankText },
    { "SetGuildBankText", &Script_SetGuildBankText },
};

// VERIFY: 91a3bc69 auto
// OFFSET: 0x005A46A0
void GuildBankRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
