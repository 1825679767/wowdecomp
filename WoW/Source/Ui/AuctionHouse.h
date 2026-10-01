#ifndef WOW_SOURCE_UI_AUCTIONHOUSE_H
#define WOW_SOURCE_UI_AUCTIONHOUSE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
class CGItem_C;
struct lua_State;

struct AuctionSortCriteria {
    /* 0000 */ int32_t m_column;
    /* 0004 */ int32_t m_reverse;
    /* 0008 */ int32_t m_valid;
};

struct AuctionSellItem {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008[2];
};

struct OutbidItemInfo {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ int32_t unk_0004;
};

struct AuctionItemClass {
    /* 0000 */ int32_t m_classID;
    /* 0004 */ int32_t m_hasSubClasses;
};

struct AuctionInvType {
    /* 0000 */ int32_t m_invType;
    /* 0004 */ char m_token[32];
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
};

struct AuctionItemInfo {
    /* 0000 */ int32_t m_auctionHouseID;
    /* 0004 */ int32_t m_auctionID;
    /* 0008 */ int32_t m_itemID;
    /* 000C */ int32_t m_enchantID[7];
    /* 0028 */ int32_t m_enchantDuration[7];
    /* 0044 */ int32_t m_enchantCharges[7];
    /* 0060 */ int32_t m_randomPropertyID;
    /* 0064 */ int32_t m_suffixFactor;
    /* 0068 */ int32_t m_count;
    /* 006C */ int32_t m_spellCharges;
    /* 0070 */ int32_t m_flags;
    /* 0074 */ int32_t unk_0074;
    /* 0078 */ WOWGUID m_owner;
    /* 0080 */ int32_t m_minBid;
    /* 0084 */ int32_t m_minIncrement;
    /* 0088 */ int32_t m_buyout;
    /* 008C */ uint32_t m_expireTime;
    /* 0090 */ WOWGUID m_bidder;
    /* 0098 */ int32_t m_bid;
    /* 009C */ int32_t m_sold;
};

class CGAuctionHouse {
    public:
    static void CloseAuctionHouse();
    static bool IsItemForSell(WOWGUID item);
    static AuctionSortCriteria m_bidSortCriteria[12];
    static AuctionSortCriteria m_ownedSortCriteria[12];
    static AuctionSortCriteria m_listSortCriteria[12];
    static int32_t m_numOutbidItems;
    static int32_t unk_C0F418;
    static TSGrowableArray<OutbidItemInfo*> m_outbidItems;
    static void SetBrowseSortCriteria(int32_t column, bool reverse);
    static void SetOwnedSortCriteria(int32_t column, bool reverse);
    static void SetBidSortCriteria(int32_t column, bool reverse);
    static WOWGUID m_auctioneer;
    static uint8_t m_sellItemSlot;
    static WOWGUID m_sellItem;
    static TSGrowableArray<AuctionSellItem> m_sellItems;
    static int32_t m_havePendingSell;
    static WOWGUID m_sellItemContainer;
    static uint32_t m_lastBrowseQueryTime;
    static uint32_t m_lastBidderQueryTime;
    static uint32_t m_lastOwnerQueryTime;
    static int32_t m_inQuery;
    static int32_t m_bidderQueriesPending;
    static int32_t m_ownerQueriesPending;
    static uint32_t m_numTotalOwned;
    static int32_t m_canGetOwnerList;
    static int32_t unk_C0F420;
    static int32_t unk_C0F424;
    static int32_t unk_C0F428;
    static int32_t unk_C0F42C;
    static int32_t m_pendingBidItems;
    static int32_t m_desiredDelayTime;
    static int32_t m_selectedBrowseItem;
    static int32_t m_selectedOwnedItem;
    static int32_t m_selectedBidItem;
    static uint32_t m_numTotalItems;
    static uint32_t m_numTotalBid;
    static TSGrowableArray<AuctionItemInfo> m_browseList;
    static TSGrowableArray<AuctionItemInfo> m_ownerList;
    static TSGrowableArray<AuctionItemInfo> m_bidderList;
    static void EnterWorld();
    static void InitializeGame();
    static uint8_t m_auctionsTabShowing;
    static bool IsSellItemValid(CGItem_C* item, int32_t showError);
    static void GetSellItem(WOWGUID* item, uint8_t* bag, uint8_t* slot);
    static void SetSellItem(WOWGUID item, WOWGUID container, uint32_t slot);
    static int32_t m_auctionHouseID;
};

int32_t Script_CloseAuctionHouse(lua_State* L);
int32_t Script_CalculateAuctionDeposit(lua_State* L);
int32_t Script_ClickAuctionSellItemButton(lua_State* L);
int32_t Script_GetAuctionSellItemInfo(lua_State* L);
int32_t Script_StartAuction(lua_State* L);
int32_t Script_QueryAuctionItems(lua_State* L);
int32_t Script_GetOwnerAuctionItems(lua_State* L);
int32_t Script_GetBidderAuctionItems(lua_State* L);
int32_t Script_GetAuctionItemInfo(lua_State* L);
int32_t Script_GetAuctionItemLink(lua_State* L);
int32_t Script_GetAuctionItemTimeLeft(lua_State* L);
int32_t Script_PlaceAuctionBid(lua_State* L);
int32_t Script_CanSendAuctionQuery(lua_State* L);
int32_t Script_SortAuctionItems(lua_State* L);
int32_t Script_SetSelectedAuctionItem(lua_State* L);
int32_t Script_GetSelectedAuctionItem(lua_State* L);
int32_t Script_IsAuctionSortReversed(lua_State* L);
int32_t Script_GetAuctionSort(lua_State* L);
int32_t Script_SortAuctionClearSort(lua_State* L);
int32_t Script_SortAuctionSetSort(lua_State* L);
int32_t Script_SortAuctionApplySort(lua_State* L);
int32_t Script_CancelSell(lua_State* L);
void AuctionHouseRegisterScriptFunctions();

int32_t Packet_MSG_AUCTION_HELLO(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_COMMAND_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_LIST_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_OWNER_LIST_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_BIDDER_LIST_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_BIDDER_NOTIFICATION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_OWNER_NOTIFICATION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_REMOVED_NOTIFICATION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_AUCTION_LIST_PENDING_SALES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

#endif
