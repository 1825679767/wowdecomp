#include "AuctionHouse.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AuctionHouseRec.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/DB/Rec/ItemClassRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00A14F70
static const AuctionItemClass s_auctionClasses[12] = {
    { 2, 1 },
    { 4, 1 },
    { 1, 1 },
    { 0, 1 },
    { 16, 1 },
    { 7, 1 },
    { 6, 1 },
    { 11, 1 },
    { 9, 1 },
    { 3, 1 },
    { 15, 1 },
    { 12, 0 },
};

// STATIC: 0x00A14FD0
static const AuctionInvType s_auctionInvTypes[14] = {
    { 1, "INVTYPE_HEAD", 1, 0 },
    { 2, "INVTYPE_NECK", 1, 1 },
    { 3, "INVTYPE_SHOULDER", 0, 0 },
    { 4, "INVTYPE_BODY", 1, 1 },
    { 5, "INVTYPE_CHEST", 0, 0 },
    { 6, "INVTYPE_WAIST", 0, 0 },
    { 7, "INVTYPE_LEGS", 0, 0 },
    { 8, "INVTYPE_FEET", 0, 0 },
    { 9, "INVTYPE_WRIST", 0, 0 },
    { 10, "INVTYPE_HAND", 0, 0 },
    { 11, "INVTYPE_FINGER", 1, 1 },
    { 12, "INVTYPE_TRINKET", 1, 1 },
    { 16, "INVTYPE_CLOAK", 0, 1 },
    { 23, "INVTYPE_HOLDABLE", 1, 1 },
};

// VERIFY: 06078343 auto
// OFFSET: 0x0059B0B0
static int32_t Script_SetAuctionsTabShowing(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetAuctionsTabShowing(bool)");
    }

    CGAuctionHouse::m_auctionsTabShowing = lua_toboolean(L, 1);
    return 0;
}

// VERIFY: 1477dadf auto
// OFFSET: 0x0059BC90
static int32_t Script_GetAuctionHouseDepositRate(lua_State* L) {
    const AuctionHouseRec* rec = g_auctionHouseDB.GetRecord(CGAuctionHouse::m_auctionHouseID);

    if (rec) {
        lua_pushnumber(L, rec->m_depositRate);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: d9ff6315 auto
// OFFSET: 0x0059C420
static int32_t Script_GetAuctionItemClasses(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 12; i++) {
        ItemClassRec* rec = g_itemClassDB.GetRecord(s_auctionClasses[i].m_classID);

        if (rec) {
            lua_pushstring(L, rec->m_className);
            count++;
        }
    }

    return count;
}

// VERIFY: f1905a55 manual
// OFFSET: 0x0059C480
static int32_t Script_GetAuctionItemSubClasses(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAuctionItemSubClasses(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 12 || !s_auctionClasses[index].m_hasSubClasses) {
        return 0;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = i < 0 || i >= g_itemSubClassDB.m_numRecords ? nullptr : &g_itemSubClassDB.m_records[i];

        if (rec->m_classID == s_auctionClasses[index].m_classID && !(rec->m_displayFlags & 0x2)) {
            lua_checkstack(L, 1);
            lua_pushstring(L, *rec->m_verboseName ? rec->m_verboseName : rec->m_displayName);
            count++;
        }
    }

    return count;
}

// VERIFY: 5c03f091 manual
// OFFSET: 0x0059C590
static int32_t Script_GetAuctionInvTypes(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetAuctionInvTypes(classIndex, subClassIndex)");
        return 0;
    }

    uint32_t classIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t subClass = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (classIndex >= 12 || !s_auctionClasses[classIndex].m_hasSubClasses) {
        return 0;
    }

    int32_t remaining = subClass;

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = i < 0 || i >= g_itemSubClassDB.m_numRecords ? nullptr : &g_itemSubClassDB.m_records[i];

        if (rec->m_classID != s_auctionClasses[classIndex].m_classID || (rec->m_displayFlags & 0x2)) {
            continue;
        }

        if (!remaining) {
            if (!(rec->m_flags & 0x200)) {
                return 0;
            }

            break;
        }

        remaining--;
    }

    lua_checkstack(L, 28);

    for (uint32_t i = 0; i < 14; i++) {
        lua_pushstring(L, s_auctionInvTypes[i].m_token);

        if (s_auctionClasses[classIndex].m_classID == 4) {
            if (subClass) {
                if ((subClass != 1 || s_auctionInvTypes[i].m_invType != 16) && s_auctionInvTypes[i].unk_0028) {
                    lua_pushnil(L);
                    continue;
                }
            } else if (!s_auctionInvTypes[i].unk_0024) {
                lua_pushnil(L);
                continue;
            }
        }

        lua_pushnumber(L, 1.0);
    }

    return 28;
}

// VERIFY: 0a1271c4 manual
// OFFSET: 0x0059C9F0
static int32_t Script_CanCancelAuction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanCancelAuction(index)");
    }

    if (!CGAuctionHouse::m_auctioneer) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    AuctionItemInfo* auction;

    if (index >= CGAuctionHouse::m_ownerList.Count() || !(auction = &CGAuctionHouse::m_ownerList.m_data[index]) || auction->m_sold) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: b8349a45 manual
// OFFSET: 0x0059DDF0
static int32_t Script_CancelAuction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedAuctionItem(index)");
    }

    if (!CGGameUI::CanPerformAction(8)) {
        return 0;
    }

    WOWGUID auctioneer = CGAuctionHouse::m_auctioneer;

    if (!auctioneer) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGAuctionHouse::m_ownerList.Count()) {
        return 0;
    }

    AuctionItemInfo* auction = &CGAuctionHouse::m_ownerList.m_data[index];

    if (!auction || auction->m_sold) {
        return 0;
    }

    if (auction->m_bid) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player || player->GetCoinage() < static_cast<uint32_t>(auction->m_bid * 0.05f)) {
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            return 0;
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AUCTION_REMOVE_ITEM));
    msg.Put(auctioneer.m_guid);
    msg.Put(static_cast<uint32_t>(auction->m_auctionID));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: 370de460 manual
// OFFSET: 0x0059C1A0
static int32_t Script_GetNumAuctionItems(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetNumAuctionItems(\"type\")");
    }

    const char* type = lua_tolstring(L, 1, nullptr);
    uint32_t total;

    if (!SStrCmpI(type, "owner", STORM_MAX_STR)) {
        lua_pushnumber(L, CGAuctionHouse::m_ownerList.Count());
        total = CGAuctionHouse::m_ownerList.Count();
    } else if (!SStrCmpI(type, "bidder", STORM_MAX_STR)) {
        lua_pushnumber(L, CGAuctionHouse::m_bidderList.Count());
        total = CGAuctionHouse::m_numTotalBid;
    } else if (!SStrCmpI(type, "list", STORM_MAX_STR)) {
        lua_pushnumber(L, CGAuctionHouse::m_browseList.Count());
        total = CGAuctionHouse::m_numTotalItems;
    } else {
        return 2;
    }

    lua_pushnumber(L, total);
    return 2;
}

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
