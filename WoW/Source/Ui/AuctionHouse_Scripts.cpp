#include "AuctionHouse.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF638
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseAuctionHouse", &Script_CloseAuctionHouse },
    { "GetAuctionHouseDepositRate", &Script_GetAuctionHouseDepositRate },
    { "CalculateAuctionDeposit", &Script_CalculateAuctionDeposit },
    { "ClickAuctionSellItemButton", &Script_ClickAuctionSellItemButton },
    { "GetAuctionSellItemInfo", &Script_GetAuctionSellItemInfo },
    { "StartAuction", &Script_StartAuction },
    { "QueryAuctionItems", &Script_QueryAuctionItems },
    { "GetOwnerAuctionItems", &Script_GetOwnerAuctionItems },
    { "GetBidderAuctionItems", &Script_GetBidderAuctionItems },
    { "GetNumAuctionItems", &Script_GetNumAuctionItems },
    { "GetAuctionItemInfo", &Script_GetAuctionItemInfo },
    { "GetAuctionItemLink", &Script_GetAuctionItemLink },
    { "GetAuctionItemTimeLeft", &Script_GetAuctionItemTimeLeft },
    { "PlaceAuctionBid", &Script_PlaceAuctionBid },
    { "GetAuctionItemClasses", &Script_GetAuctionItemClasses },
    { "GetAuctionItemSubClasses", &Script_GetAuctionItemSubClasses },
    { "GetAuctionInvTypes", &Script_GetAuctionInvTypes },
    { "CanSendAuctionQuery", &Script_CanSendAuctionQuery },
    { "SortAuctionItems", &Script_SortAuctionItems },
    { "SetSelectedAuctionItem", &Script_SetSelectedAuctionItem },
    { "GetSelectedAuctionItem", &Script_GetSelectedAuctionItem },
    { "IsAuctionSortReversed", &Script_IsAuctionSortReversed },
    { "CancelAuction", &Script_CancelAuction },
    { "CanCancelAuction", &Script_CanCancelAuction },
    { "GetAuctionSort", &Script_GetAuctionSort },
    { "SortAuctionClearSort", &Script_SortAuctionClearSort },
    { "SortAuctionSetSort", &Script_SortAuctionSetSort },
    { "SortAuctionApplySort", &Script_SortAuctionApplySort },
    { "CancelSell", &Script_CancelSell },
    { "SetAuctionsTabShowing", &Script_SetAuctionsTabShowing },
};

// VERIFY: 1cd28449 auto
// OFFSET: 0x0059B100
void AuctionHouseRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
