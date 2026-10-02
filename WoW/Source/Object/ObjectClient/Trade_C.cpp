#include "Trade_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"

// STATIC: 0x00CA0B7C
uint32_t s_clientStateIndex;

// STATIC: 0x00CA0FE0
static int32_t s_unk_CA0FE0;

// STATIC: 0x00CA0FE4
static int32_t s_unk_CA0FE4;

// STATIC: 0x00CA0FE8
static WOWGUID s_unk_CA0FE8;

// STATIC: 0x00CA0FF0
static int32_t s_tradeEnchantSpell[2];

// STATIC: 0x00CA0FF8
static uint32_t s_nextTradeRequestTime;

// VERIFY: 71c6ebe1 auto
// OFFSET: 0x00703C50
int32_t Trade_C_GetEnchantSpell(int32_t player) {
    return s_tradeEnchantSpell[player];
}

// VERIFY: c648ea3e auto
// OFFSET: 0x00703C60
static void TradeNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, nullptr, nullptr, false);

    if (name) {
        CGGameUI::DisplayError(ERR_INITIATE_TRADE_S, name->m_name);
    }
}

// VERIFY: ef2d8e5d auto
// OFFSET: 0x00703CD0
void Trade_C_Destroy() {
    ClientServices::ClearMessageHandler(SMSG_TRADE_STATUS);
    ClientServices::ClearMessageHandler(SMSG_TRADE_STATUS_EXTENDED);
}

// VERIFY: f2899d59 manual
// OFFSET: 0x00703CF0
void CGTradeInfo::InitiateTrade(WOWGUID target, int32_t a2) {
    if (s_unk_CA0FE8) {
        CGGameUI::DisplayError(ERR_ALREADY_TRADING);
        return;
    }

    if (s_nextTradeRequestTime && static_cast<int32_t>(FrameTime::s_curTimeMs - s_nextTradeRequestTime) < 0) {
        return;
    }

    s_nextTradeRequestTime = FrameTime::s_curTimeMs + 1000;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_INITIATE_TRADE));
    msg.Put(target.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_unk_CA0FE4 = a2;
    s_unk_CA0FE8 = target;
    s_unk_CA0FE0 = 1;
    NameCache* name = g_nameDBCache.GetRecord(target.m_guid, target, &TradeNameCallback, nullptr, false);

    if (name) {
        CGGameUI::DisplayError(ERR_INITIATE_TRADE_S, name->m_name);
    }
}

// VERIFY: cc5d4c9b manual
// OFFSET: 0x00703E00
void Trade_C_BeginTrade() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BEGIN_TRADE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: ab43a905 manual
// OFFSET: 0x00703E70
void Trade_C_PlayerBusy() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BUSY_TRADE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: b9daa0d2 manual
// OFFSET: 0x00703EE0
void Trade_C_PlayerIgnored() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_IGNORE_TRADE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: db33ff5b manual
// OFFSET: 0x00703F50
void Trade_C_AcceptTrade() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACCEPT_TRADE));
    msg.Put(CGTradeInfo::m_playerTrade.unk_0000[2]);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: f77c5a3f manual
// OFFSET: 0x00703FD0
void Trade_C_UnacceptTrade() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UNACCEPT_TRADE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: d10db303 manual
// OFFSET: 0x00704040
void Trade_C_CancelTrade() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_TRADE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 805652c0 manual
// OFFSET: 0x007040B0
bool Trade_C_AddItem(WOWGUID item, WOWGUID bag, uint8_t slot, uint8_t tradeSlot) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__)) {
        return false;
    }

    uint8_t bagIndex = player->FindSlotIndex(bag);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TRADE_ITEM));
    msg.Put(tradeSlot);
    msg.Put(bagIndex);
    msg.Put(slot);
    s_clientStateIndex++;
    msg.Finalize();
    ClientServices::Send2(&msg);

    return true;
}

// VERIFY: a89b3295 manual
// OFFSET: 0x007041A0
void Trade_C_RemoveItem(uint8_t tradeSlot) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CLEAR_TRADE_ITEM));
    msg.Put(tradeSlot);
    s_clientStateIndex++;
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 9b901e66 manual
// OFFSET: 0x00704220
bool CGTradeInfo::AddMoney(int32_t amount) {
    if (!amount) {
        return true;
    }

    uint32_t total = CGTradeInfo::m_playerTrade.m_money + amount;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->GetCoinage() < total) {
        return false;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TRADE_GOLD));
    msg.Put(total);
    s_clientStateIndex++;
    msg.Finalize();
    CGTradeInfo::m_playerTrade.m_money = total;
    ClientServices::Send2(&msg);
    return true;
}

// VERIFY: 01456882 manual
// OFFSET: 0x007043C0
bool CGTradeInfo::SetMoney(uint32_t amount) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->GetCoinage() < amount) {
        return false;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TRADE_GOLD));
    msg.Put(amount);
    s_clientStateIndex++;
    msg.Finalize();
    CGTradeInfo::m_playerTrade.m_money = amount;
    ClientServices::Send2(&msg);
    return true;
}

// VERIFY: c435de61 auto
// OFFSET: 0x007048D0
void Trade_C_Initialize() {
    s_unk_CA0FE0 = 0;
    s_unk_CA0FE4 = 0;
    s_unk_CA0FE8 = WOWGUID();
    ClientServices::SetMessageHandler(SMSG_TRADE_STATUS, Packet_SMSG_TRADE_STATUS, nullptr);
    ClientServices::SetMessageHandler(SMSG_TRADE_STATUS_EXTENDED, Packet_SMSG_TRADE_STATUS_EXTENDED, nullptr);
}
