#ifndef WOW_SOURCE_UI_TRADEFRAME_H
#define WOW_SOURCE_UI_TRADEFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGItem_C;

struct lua_State;

struct PlayerTradeState {
    /* 0000 */ uint32_t unk_0000[3];
    /* 000C */ uint32_t m_money;
    /* 0010 */ uint32_t m_targetMoney;
};

class CGTradeInfo {
    public:
    static void SetTradePartner(WOWGUID partner, int32_t a2);
    static void SetEnchantingItem(CGItem_C* item);
    static void AddPlayerItem(WOWGUID item, WOWGUID container, uint32_t slot);
    static WOWGUID m_enchantingItem;
    static void EnterWorld();
    static uint32_t GetPlayerTradeMoney();
    static uint32_t GetTargetTradeMoney();
    static WOWGUID m_tradingPlayer;
    static PlayerTradeState m_playerTrade;
    static int32_t m_targetDurability[7];
    static int32_t m_targetMaxDurability[7];
    static int32_t m_targetLockID[7];
    static int32_t m_targetRandomPropertyID[7];
    static int32_t m_targetSuffixFactor[7];
    static int32_t m_targetSpellCharges[7];
    static WOWGUID m_targetGiftCreator[7];
    static int32_t m_targetWrapped[7];
    static int32_t m_targetEnchant[7];
    static WOWGUID m_targetCreator[7];
    static int32_t m_targetSocketEnchants[7][3];
    static int32_t m_targetCount[7];
    static int32_t m_targetItemID[7];
    static uint8_t unk_BFA5DC[7];
    static WOWGUID unk_BFA5E8[7];
    static WOWGUID m_playerItems[7];
    static int32_t m_playerAccepted;
    static int32_t m_targetAccepted;
    static int32_t unk_BFA668;
    static int32_t unk_BFA66C;
    static int32_t unk_BFA670;
    static int32_t unk_BFA674;

    static void UpdatePlayerItem(WOWGUID guid);
    static void AddCursorItem(int32_t bound);
    static void InitiateTrade(WOWGUID guid, int32_t a2);
};

int32_t Script_CloseTrade(lua_State* L);
int32_t Script_ClickTradeButton(lua_State* L);
int32_t Script_ClickTargetTradeButton(lua_State* L);
int32_t Script_GetTradeTargetItemInfo(lua_State* L);
int32_t Script_GetTradeTargetItemLink(lua_State* L);
int32_t Script_GetTradePlayerItemInfo(lua_State* L);
int32_t Script_GetTradePlayerItemLink(lua_State* L);
int32_t Script_AcceptTrade(lua_State* L);
int32_t Script_PickupTradeMoney(lua_State* L);
int32_t Script_AddTradeMoney(lua_State* L);
int32_t Script_SetTradeMoney(lua_State* L);
void TradeInfoRegisterScriptFunctions();

#endif
