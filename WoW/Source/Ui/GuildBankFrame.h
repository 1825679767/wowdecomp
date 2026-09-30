#ifndef WOW_SOURCE_UI_GUILDBANKFRAME_H
#define WOW_SOURCE_UI_GUILDBANKFRAME_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
struct lua_State;

struct GuildBankTabInfo {
    /* 0000 */ char m_name[64];
    /* 0040 */ char m_icon[256];
    /* 0140 */ char m_text[8192];
    /* 2140 */ int32_t unk_2140;
};

int32_t Packet_SMSG_GUILD_BANK_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_BANK_LOG_QUERY(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_GUILD_BANK_MONEY_WITHDRAWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_QUERY_GUILD_BANK_TEXT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGGuildBankInfo {
    public:
    static WOWGUID m_banker;
    static WOWGUID m_pendingBanker;
    static void SetGuildBanker(const WOWGUID& banker);
    static int32_t m_numMoneyTransactions;
    static uint8_t m_numTabs;
    static GuildBankTabInfo m_guildBankTabInfo[6];
    static uint8_t m_currentTab;
    static int32_t m_remainingWithdrawMoney;
    static uint64_t m_money;
    static void EnterWorld();
    static void InitializeGame();
    static void ClearAllTabs();
    static void Unlock(int32_t tab, uint32_t slot);
};

int32_t Script_QueryGuildBankTab(lua_State* L);
int32_t Script_GetGuildBankItemInfo(lua_State* L);
int32_t Script_SetGuildBankTabInfo(lua_State* L);
int32_t Script_GetGuildBankItemLink(lua_State* L);
int32_t Script_PickupGuildBankItem(lua_State* L);
int32_t Script_AutoStoreGuildBankItem(lua_State* L);
int32_t Script_SplitGuildBankItem(lua_State* L);
int32_t Script_GetGuildBankTabInfo(lua_State* L);
int32_t Script_BuyGuildBankTab(lua_State* L);
int32_t Script_DepositGuildBankMoney(lua_State* L);
int32_t Script_WithdrawGuildBankMoney(lua_State* L);
int32_t Script_PickupGuildBankMoney(lua_State* L);
int32_t Script_CloseGuildBankFrame(lua_State* L);
int32_t Script_GetGuildTabardFileNames(lua_State* L);
int32_t Script_QueryGuildBankLog(lua_State* L);
int32_t Script_GetNumGuildBankTransactions(lua_State* L);
int32_t Script_GetGuildBankTransaction(lua_State* L);
int32_t Script_GetGuildBankMoneyTransaction(lua_State* L);
int32_t Script_QueryGuildBankText(lua_State* L);
int32_t Script_GetGuildBankText(lua_State* L);
int32_t Script_SetGuildBankText(lua_State* L);
void GuildBankRegisterScriptFunctions();

#endif
