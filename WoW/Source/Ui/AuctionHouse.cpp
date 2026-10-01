#include "AuctionHouse.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C0E760
TSGrowableArray<AuctionSellItem> CGAuctionHouse::m_sellItems;

// STATIC: 0x00C0F434
int32_t CGAuctionHouse::m_havePendingSell;

// STATIC: 0x00C0F3F5
uint8_t CGAuctionHouse::m_auctionsTabShowing;

// STATIC: 0x00C0F3F0
int32_t CGAuctionHouse::m_auctionHouseID;

// STATIC: 0x00C0F3E8
WOWGUID CGAuctionHouse::m_auctioneer;

// STATIC: 0x00C0F408
uint32_t CGAuctionHouse::m_numTotalItems;

// STATIC: 0x00C0F410
uint32_t CGAuctionHouse::m_numTotalBid;

// STATIC: 0x00C0F440
TSGrowableArray<AuctionItemInfo> CGAuctionHouse::m_browseList;

// STATIC: 0x00C0F450
TSGrowableArray<AuctionItemInfo> CGAuctionHouse::m_ownerList;

// STATIC: 0x00C0F460
TSGrowableArray<AuctionItemInfo> CGAuctionHouse::m_bidderList;

// STATIC: 0x00ACF628
int32_t CGAuctionHouse::m_desiredDelayTime = 5000;

// STATIC: 0x00ACF62C
int32_t CGAuctionHouse::m_selectedBrowseItem = -1;

// STATIC: 0x00ACF630
int32_t CGAuctionHouse::m_selectedOwnedItem = -1;

// STATIC: 0x00ACF634
int32_t CGAuctionHouse::m_selectedBidItem = -1;

// STATIC: 0x00C0E728
uint32_t CGAuctionHouse::m_lastBrowseQueryTime;

// STATIC: 0x00C0E72C
uint32_t CGAuctionHouse::m_lastBidderQueryTime;

// STATIC: 0x00C0E730
uint32_t CGAuctionHouse::m_lastOwnerQueryTime;

// STATIC: 0x00C0E734
int32_t CGAuctionHouse::m_inQuery;

// STATIC: 0x00C0E738
int32_t CGAuctionHouse::m_bidderQueriesPending;

// STATIC: 0x00C0E73C
int32_t CGAuctionHouse::m_ownerQueriesPending;

// STATIC: 0x00C0F3F4
uint8_t CGAuctionHouse::m_sellItemSlot;

// STATIC: 0x00C0F3F8
WOWGUID CGAuctionHouse::m_sellItem;

// STATIC: 0x00C0F400
WOWGUID CGAuctionHouse::m_sellItemContainer;

// STATIC: 0x00C0F40C
uint32_t CGAuctionHouse::m_numTotalOwned;

// STATIC: 0x00C0F41C
int32_t CGAuctionHouse::m_canGetOwnerList;

// STATIC: 0x00C0F420
int32_t CGAuctionHouse::unk_C0F420;

// STATIC: 0x00C0F424
int32_t CGAuctionHouse::unk_C0F424;

// STATIC: 0x00C0F428
int32_t CGAuctionHouse::unk_C0F428;

// STATIC: 0x00C0F42C
int32_t CGAuctionHouse::unk_C0F42C;

// STATIC: 0x00C0F430
int32_t CGAuctionHouse::m_pendingBidItems;

// STATIC: 0x00C0E578
AuctionSortCriteria CGAuctionHouse::m_bidSortCriteria[12];

// STATIC: 0x00C0E608
AuctionSortCriteria CGAuctionHouse::m_ownedSortCriteria[12];

// STATIC: 0x00C0E698
AuctionSortCriteria CGAuctionHouse::m_listSortCriteria[12];

// STATIC: 0x00C0F414
int32_t CGAuctionHouse::m_numOutbidItems;

// STATIC: 0x00C0F418
int32_t CGAuctionHouse::unk_C0F418;

// STATIC: 0x00C0F470
TSGrowableArray<OutbidItemInfo*> CGAuctionHouse::m_outbidItems;

// VERIFY: 0265457d auto
// OFFSET: 0x0059DFB0
void CGAuctionHouse::EnterWorld() {
    if (CGAuctionHouse::m_sellItem) {
        CGGameUI::UnlockItem(CGAuctionHouse::m_sellItem);
        CGAuctionHouse::m_sellItem = WOWGUID();
        CGAuctionHouse::m_sellItemContainer = WOWGUID();
        CGAuctionHouse::m_sellItemSlot = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_NEW_AUCTION_UPDATE, nullptr);
    }

    CGAuctionHouse::m_lastBrowseQueryTime = 0;
    CGAuctionHouse::m_lastBidderQueryTime = 0;
    CGAuctionHouse::m_lastOwnerQueryTime = 0;
    CGAuctionHouse::m_inQuery = 0;
    CGAuctionHouse::m_bidderQueriesPending = 0;
    CGAuctionHouse::m_ownerQueriesPending = 0;
    CGAuctionHouse::m_auctioneer = WOWGUID();
    CGAuctionHouse::m_auctionHouseID = 0;
    CGAuctionHouse::m_browseList.m_count = 0;
    CGAuctionHouse::m_numTotalItems = 0;
    CGAuctionHouse::m_numTotalOwned = 0;
    CGAuctionHouse::m_numTotalBid = 0;
    CGAuctionHouse::m_canGetOwnerList = 0;
    CGAuctionHouse::unk_C0F420 = 0;
    CGAuctionHouse::unk_C0F424 = 0;
    CGAuctionHouse::unk_C0F428 = 0;
    CGAuctionHouse::unk_C0F42C = 0;
    CGAuctionHouse::m_pendingBidItems = 0;
    CGAuctionHouse::m_auctionsTabShowing = 0;
    CGAuctionHouse::m_desiredDelayTime = 5000;
    CGAuctionHouse::m_selectedBrowseItem = -1;
    CGAuctionHouse::m_selectedOwnedItem = -1;
    CGAuctionHouse::m_selectedBidItem = -1;
}

// VERIFY: cb1e4eed manual
// OFFSET: 0x0059A5E0
void CGAuctionHouse::SetBrowseSortCriteria(int32_t column, bool reverse) {
    uint32_t index = 0;

    while (CGAuctionHouse::m_listSortCriteria[index].m_column != column && CGAuctionHouse::m_listSortCriteria[index].m_valid) {
        index++;

        if (index >= 12) {
            return;
        }
    }

    for (; index > 0; index--) {
        CGAuctionHouse::m_listSortCriteria[index] = CGAuctionHouse::m_listSortCriteria[index - 1];
    }

    CGAuctionHouse::m_listSortCriteria[0].m_column = column;
    CGAuctionHouse::m_listSortCriteria[0].m_reverse = reverse;
    CGAuctionHouse::m_listSortCriteria[0].m_valid = 1;
}

// VERIFY: 33383a3f manual
// OFFSET: 0x0059A690
void CGAuctionHouse::SetOwnedSortCriteria(int32_t column, bool reverse) {
    uint32_t index = 0;

    while (CGAuctionHouse::m_ownedSortCriteria[index].m_column != column && CGAuctionHouse::m_ownedSortCriteria[index].m_valid) {
        index++;

        if (index >= 12) {
            return;
        }
    }

    for (; index > 0; index--) {
        CGAuctionHouse::m_ownedSortCriteria[index] = CGAuctionHouse::m_ownedSortCriteria[index - 1];
    }

    CGAuctionHouse::m_ownedSortCriteria[0].m_column = column;
    CGAuctionHouse::m_ownedSortCriteria[0].m_reverse = reverse;
    CGAuctionHouse::m_ownedSortCriteria[0].m_valid = 1;
}

// VERIFY: dcb842e3 manual
// OFFSET: 0x0059A740
void CGAuctionHouse::SetBidSortCriteria(int32_t column, bool reverse) {
    uint32_t index = 0;

    while (CGAuctionHouse::m_bidSortCriteria[index].m_column != column && CGAuctionHouse::m_bidSortCriteria[index].m_valid) {
        index++;

        if (index >= 12) {
            return;
        }
    }

    for (; index > 0; index--) {
        CGAuctionHouse::m_bidSortCriteria[index] = CGAuctionHouse::m_bidSortCriteria[index - 1];
    }

    CGAuctionHouse::m_bidSortCriteria[0].m_column = column;
    CGAuctionHouse::m_bidSortCriteria[0].m_reverse = reverse;
    CGAuctionHouse::m_bidSortCriteria[0].m_valid = 1;
}

// VERIFY: 62108de1 manual
// OFFSET: 0x005A0BA0
void CGAuctionHouse::InitializeGame() {
    ClientServices::SetMessageHandler(MSG_AUCTION_HELLO, Packet_MSG_AUCTION_HELLO, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_COMMAND_RESULT, Packet_SMSG_AUCTION_COMMAND_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_LIST_RESULT, Packet_SMSG_AUCTION_LIST_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_LIST_OWNER_ITEMS_RESULT, Packet_SMSG_AUCTION_OWNER_LIST_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_LIST_BIDDER_ITEMS_RESULT, Packet_SMSG_AUCTION_BIDDER_LIST_RESULT, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_BIDDER_NOTIFICATION, Packet_SMSG_AUCTION_BIDDER_NOTIFICATION, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_OWNER_NOTIFICATION, Packet_SMSG_AUCTION_OWNER_NOTIFICATION, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_REMOVED_NOTIFICATION, Packet_SMSG_AUCTION_REMOVED_NOTIFICATION, nullptr);
    ClientServices::SetMessageHandler(SMSG_AUCTION_LIST_PENDING_SALES, Packet_SMSG_AUCTION_LIST_PENDING_SALES, nullptr);

    CGAuctionHouse::m_ownerList.SetCount(0);
    CGAuctionHouse::m_bidderList.SetCount(0);
    CGAuctionHouse::m_browseList.SetCount(0);
    CGAuctionHouse::unk_C0F418 = 0;
    CGAuctionHouse::m_lastBrowseQueryTime = 0;
    CGAuctionHouse::m_lastBidderQueryTime = 0;
    CGAuctionHouse::m_lastOwnerQueryTime = 0;
    CGAuctionHouse::m_inQuery = 0;
    CGAuctionHouse::m_bidderQueriesPending = 0;
    CGAuctionHouse::m_ownerQueriesPending = 0;
    CGAuctionHouse::m_numOutbidItems = 0;

    CGAuctionHouse::m_outbidItems.SetCount(8);

    for (uint32_t i = 0; i < 8; i++) {
        CGAuctionHouse::m_outbidItems.m_data[i] = static_cast<OutbidItemInfo*>(SMemAlloc(sizeof(OutbidItemInfo), __FILE__, __LINE__, 0));
    }

    for (uint32_t i = 0; i < 12; i++) {
        CGAuctionHouse::m_listSortCriteria[i].m_valid = 0;
    }

    CGAuctionHouse::SetBrowseSortCriteria(3, false);
    CGAuctionHouse::SetBrowseSortCriteria(8, false);
    CGAuctionHouse::SetBrowseSortCriteria(9, true);
    CGAuctionHouse::SetBrowseSortCriteria(6, false);
    CGAuctionHouse::SetBrowseSortCriteria(5, false);
    CGAuctionHouse::SetBrowseSortCriteria(0, true);
    CGAuctionHouse::SetBrowseSortCriteria(1, false);

    for (uint32_t i = 0; i < 12; i++) {
        CGAuctionHouse::m_ownedSortCriteria[i].m_valid = 0;
    }

    CGAuctionHouse::SetOwnedSortCriteria(9, true);
    CGAuctionHouse::SetOwnedSortCriteria(8, false);
    CGAuctionHouse::SetOwnedSortCriteria(5, false);
    CGAuctionHouse::SetOwnedSortCriteria(0, true);
    CGAuctionHouse::SetOwnedSortCriteria(1, false);
    CGAuctionHouse::SetOwnedSortCriteria(4, false);
    CGAuctionHouse::SetOwnedSortCriteria(3, false);

    for (uint32_t i = 0; i < 12; i++) {
        CGAuctionHouse::m_bidSortCriteria[i].m_valid = 0;
    }

    CGAuctionHouse::SetBidSortCriteria(9, false);
    CGAuctionHouse::SetBidSortCriteria(5, false);
    CGAuctionHouse::SetBidSortCriteria(0, true);
    CGAuctionHouse::SetBidSortCriteria(1, false);
    CGAuctionHouse::SetBidSortCriteria(6, false);
    CGAuctionHouse::SetBidSortCriteria(4, false);
    CGAuctionHouse::SetBidSortCriteria(8, false);
    CGAuctionHouse::SetBidSortCriteria(3, false);
}

// VERIFY: 90d31b32 manual
// OFFSET: 0x0059AA50
bool CGAuctionHouse::IsItemForSell(WOWGUID item) {
    if (!CGAuctionHouse::m_havePendingSell) {
        return false;
    }

    for (uint32_t i = 0; i < CGAuctionHouse::m_sellItems.Count(); i++) {
        if (CGAuctionHouse::m_sellItems[i].m_guid == item) {
            return true;
        }
    }

    return false;
}

// VERIFY: 93924a65 manual
// OFFSET: 0x0059E090
void CGAuctionHouse::ShutdownGame() {
    ClientServices::ClearMessageHandler(MSG_AUCTION_HELLO);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_COMMAND_RESULT);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_LIST_RESULT);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_LIST_OWNER_ITEMS_RESULT);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_LIST_BIDDER_ITEMS_RESULT);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_BIDDER_NOTIFICATION);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_OWNER_NOTIFICATION);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_REMOVED_NOTIFICATION);
    ClientServices::ClearMessageHandler(SMSG_AUCTION_LIST_PENDING_SALES);

    for (uint32_t i = 0; i < CGAuctionHouse::m_outbidItems.m_count; i++) {
        delete CGAuctionHouse::m_outbidItems.m_data[i];
    }

    CGAuctionHouse::m_outbidItems.Clear();
    CGAuctionHouse::m_numOutbidItems = 0;
}
