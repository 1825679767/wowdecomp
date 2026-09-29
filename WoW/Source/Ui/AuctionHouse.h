#ifndef WOW_SOURCE_UI_AUCTIONHOUSE_H
#define WOW_SOURCE_UI_AUCTIONHOUSE_H

#include <cstdint>

struct lua_State;

class CGAuctionHouse {
    public:
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_CloseAuctionHouse(lua_State* L);
int32_t Script_GetAuctionHouseDepositRate(lua_State* L);
int32_t Script_CalculateAuctionDeposit(lua_State* L);
int32_t Script_ClickAuctionSellItemButton(lua_State* L);
int32_t Script_GetAuctionSellItemInfo(lua_State* L);
int32_t Script_StartAuction(lua_State* L);
int32_t Script_QueryAuctionItems(lua_State* L);
int32_t Script_GetOwnerAuctionItems(lua_State* L);
int32_t Script_GetBidderAuctionItems(lua_State* L);
int32_t Script_GetNumAuctionItems(lua_State* L);
int32_t Script_GetAuctionItemInfo(lua_State* L);
int32_t Script_GetAuctionItemLink(lua_State* L);
int32_t Script_GetAuctionItemTimeLeft(lua_State* L);
int32_t Script_PlaceAuctionBid(lua_State* L);
int32_t Script_GetAuctionItemClasses(lua_State* L);
int32_t Script_GetAuctionItemSubClasses(lua_State* L);
int32_t Script_GetAuctionInvTypes(lua_State* L);
int32_t Script_CanSendAuctionQuery(lua_State* L);
int32_t Script_SortAuctionItems(lua_State* L);
int32_t Script_SetSelectedAuctionItem(lua_State* L);
int32_t Script_GetSelectedAuctionItem(lua_State* L);
int32_t Script_IsAuctionSortReversed(lua_State* L);
int32_t Script_CancelAuction(lua_State* L);
int32_t Script_CanCancelAuction(lua_State* L);
int32_t Script_GetAuctionSort(lua_State* L);
int32_t Script_SortAuctionClearSort(lua_State* L);
int32_t Script_SortAuctionSetSort(lua_State* L);
int32_t Script_SortAuctionApplySort(lua_State* L);
int32_t Script_CancelSell(lua_State* L);
int32_t Script_SetAuctionsTabShowing(lua_State* L);
void AuctionHouseRegisterScriptFunctions();

#endif
