#include "Trade_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA0B7C
uint32_t s_clientStateIndex;

// VERIFY: ef2d8e5d auto
// OFFSET: 0x00703CD0
void Trade_C_Destroy() {
    ClientServices::ClearMessageHandler(SMSG_TRADE_STATUS);
    ClientServices::ClearMessageHandler(SMSG_TRADE_STATUS_EXTENDED);
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
