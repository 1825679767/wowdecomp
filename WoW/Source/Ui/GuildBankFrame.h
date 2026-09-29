#ifndef WOW_SOURCE_UI_GUILDBANKFRAME_H
#define WOW_SOURCE_UI_GUILDBANKFRAME_H

#include <cstdint>

struct lua_State;

class CGGuildBankInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
    static void ClearAllTabs();
    static void Unlock(int32_t tab, uint32_t slot);
};

int32_t Script_QueryGuildBankTab(lua_State* L);
int32_t Script_SetCurrentGuildBankTab(lua_State* L);
int32_t Script_GetCurrentGuildBankTab(lua_State* L);
int32_t Script_GetGuildBankItemInfo(lua_State* L);
int32_t Script_SetGuildBankTabInfo(lua_State* L);
int32_t Script_GetGuildBankItemLink(lua_State* L);
int32_t Script_PickupGuildBankItem(lua_State* L);
int32_t Script_AutoStoreGuildBankItem(lua_State* L);
int32_t Script_SplitGuildBankItem(lua_State* L);
int32_t Script_GetNumGuildBankTabs(lua_State* L);
int32_t Script_GetGuildBankTabInfo(lua_State* L);
int32_t Script_GetGuildBankTabCost(lua_State* L);
int32_t Script_BuyGuildBankTab(lua_State* L);
int32_t Script_DepositGuildBankMoney(lua_State* L);
int32_t Script_WithdrawGuildBankMoney(lua_State* L);
int32_t Script_CanWithdrawGuildBankMoney(lua_State* L);
int32_t Script_PickupGuildBankMoney(lua_State* L);
int32_t Script_GetGuildBankMoney(lua_State* L);
int32_t Script_GetGuildBankWithdrawMoney(lua_State* L);
int32_t Script_CloseGuildBankFrame(lua_State* L);
int32_t Script_GetGuildTabardFileNames(lua_State* L);
int32_t Script_QueryGuildBankLog(lua_State* L);
int32_t Script_GetNumGuildBankTransactions(lua_State* L);
int32_t Script_GetGuildBankTransaction(lua_State* L);
int32_t Script_GetNumGuildBankMoneyTransactions(lua_State* L);
int32_t Script_GetGuildBankMoneyTransaction(lua_State* L);
int32_t Script_QueryGuildBankText(lua_State* L);
int32_t Script_GetGuildBankText(lua_State* L);
int32_t Script_SetGuildBankText(lua_State* L);
void GuildBankRegisterScriptFunctions();

#endif
