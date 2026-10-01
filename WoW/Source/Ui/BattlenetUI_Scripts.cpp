#include "BattlenetUI.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00ACC9E8
static uint32_t s_presenceOnlineField = 0x10002;

// VERIFY: f52b1323 manual
// OFFSET: 0x00534590
static int32_t Script_BNGetNumFriends(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled() || !BattlenetUI::m_friendListLoaded) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    if (!BattlenetUI_IsAvailable()) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    lua_pushnumber(L, BattlenetUI::m_friends.Count());

    uint32_t online = 0;

    for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
        BattlenetPresenceValue value = BattlenetAPI::GetPresenceValue(BattlenetUI::m_friends.m_data[i].m_presenceID, s_presenceOnlineField);

        if (value.m_type == 1 && value.m_bool) {
            online++;
        }
    }

    lua_pushnumber(L, online);
    return 2;
}

// VERIFY: 32894792 auto
// OFFSET: 0x00537010
static int32_t Script_BNFeaturesEnabledAndConnected(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    lua_pushboolean(L, login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled() && static_cast<BattlenetLogin*>(login)->IsConnected());
    return 1;
}

// VERIFY: ed1e6ea4 auto
// OFFSET: 0x00537070
static int32_t Script_BNFeaturesEnabled(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    lua_pushboolean(L, login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled());
    return 1;
}

// VERIFY: 2e29ab78 auto
// OFFSET: 0x00537A00
static int32_t Script_BNGetMaxPlayersInConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    lua_pushnumber(L, 12.0);
    return 1;
}

// VERIFY: 423fdd2d manual
// OFFSET: 0x00536400
static int32_t Script_BNGetNumBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    if (!BattlenetUI_IsAvailable()) {
        lua_pushnumber(L, 0.0);
        DebugLog("BNUI: Num Blocked is 0. NOT CONNECTED");
        return 1;
    }

    BattlenetBlockList blockList = BattlenetAPI::GetBlockList();
    lua_pushnumber(L, blockList.m_blockedCount);
    DebugLog("BNUI: Num Blocked is %d", blockList.m_blockedCount);
    return 1;
}

// VERIFY: 6fff9574 manual
// OFFSET: 0x00536890
static int32_t Script_BNGetNumBlockedToons(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    if (!BattlenetUI_IsAvailable()) {
        lua_pushnumber(L, 0.0);
        DebugLog("BNUI: Num Blocked toons is 0. NOT CONNECTED");
        return 1;
    }

    BattlenetBlockList blockList = BattlenetAPI::GetBlockList();
    lua_pushnumber(L, blockList.m_blockedToonCount);
    DebugLog("BNUI: Num Blocked toons is %d", blockList.m_blockedToonCount);
    return 1;
}

// STATIC: 0x00ACCAE8
static FrameScript_Method s_ScriptFunctions[] = {
    { "BNGetInfo", &Script_BNGetInfo },
    { "BNGetNumFriends", &Script_BNGetNumFriends },
    { "BNGetFriendInfo", &Script_BNGetFriendInfo },
    { "BNGetFriendInfoByID", &Script_BNGetFriendInfoByID },
    { "BNGetNumFriendToons", &Script_BNGetNumFriendToons },
    { "BNGetFriendToonInfo", &Script_BNGetFriendToonInfo },
    { "BNGetToonInfo", &Script_BNGetToonInfo },
    { "BNRemoveFriend", &Script_BNRemoveFriend },
    { "BNSetFriendNote", &Script_BNSetFriendNote },
    { "BNSetSelectedFriend", &Script_BNSetSelectedFriend },
    { "BNGetSelectedFriend", &Script_BNGetSelectedFriend },
    { "BNGetNumFriendInvites", &Script_BNGetNumFriendInvites },
    { "BNGetFriendInviteInfo", &Script_BNGetFriendInviteInfo },
    { "BNSendFriendInvite", &Script_BNSendFriendInvite },
    { "BNSendFriendInviteByID", &Script_BNSendFriendInviteByID },
    { "BNAcceptFriendInvite", &Script_BNAcceptFriendInvite },
    { "BNDeclineFriendInvite", &Script_BNDeclineFriendInvite },
    { "BNReportFriendInvite", &Script_BNReportFriendInvite },
    { "BNSetAFK", &Script_BNSetAFK },
    { "BNSetDND", &Script_BNSetDND },
    { "BNSetCustomMessage", &Script_BNSetCustomMessage },
    { "BNGetCustomMessageTable", &Script_BNGetCustomMessageTable },
    { "BNSetFocus", &Script_BNSetFocus },
    { "BNSendWhisper", &Script_BNSendWhisper },
    { "BNCreateConversation", &Script_BNCreateConversation },
    { "BNInviteToConversation", &Script_BNInviteToConversation },
    { "BNLeaveConversation", &Script_BNLeaveConversation },
    { "BNSendConversationMessage", &Script_BNSendConversationMessage },
    { "BNGetNumConversationMembers", &Script_BNGetNumConversationMembers },
    { "BNGetConversationMemberInfo", &Script_BNGetConversationMemberInfo },
    { "BNGetConversationInfo", &Script_BNGetConversationInfo },
    { "BNListConversation", &Script_BNListConversation },
    { "BNGetNumBlocked", &Script_BNGetNumBlocked },
    { "BNGetBlockedInfo", &Script_BNGetBlockedInfo },
    { "BNIsBlocked", &Script_BNIsBlocked },
    { "BNSetBlocked", &Script_BNSetBlocked },
    { "BNSetSelectedBlock", &Script_BNSetSelectedBlock },
    { "BNGetSelectedBlock", &Script_BNGetSelectedBlock },
    { "BNGetNumBlockedToons", &Script_BNGetNumBlockedToons },
    { "BNGetBlockedToonInfo", &Script_BNGetBlockedToonInfo },
    { "BNIsToonBlocked", &Script_BNIsToonBlocked },
    { "BNSetToonBlocked", &Script_BNSetToonBlocked },
    { "BNSetSelectedToonBlock", &Script_BNSetSelectedToonBlock },
    { "BNGetSelectedToonBlock", &Script_BNGetSelectedToonBlock },
    { "BNReportPlayer", &Script_BNReportPlayer },
    { "BNConnected", &Script_BNConnected },
    { "BNFeaturesEnabledAndConnected", &Script_BNFeaturesEnabledAndConnected },
    { "IsBNLogin", &Script_IsBNLogin },
    { "BNFeaturesEnabled", &Script_BNFeaturesEnabled },
    { "BNRequestFOFInfo", &Script_BNRequestFOFInfo },
    { "BNGetNumFOF", &Script_BNGetNumFOF },
    { "BNGetFOFInfo", &Script_BNGetFOFInfo },
    { "BNSetMatureLanguageFilter", &Script_BNSetMatureLanguageFilter },
    { "BNGetMatureLanguageFilter", &Script_BNGetMatureLanguageFilter },
    { "BNIsSelf", &Script_BNIsSelf },
    { "BNIsFriend", &Script_BNIsFriend },
    { "BNGetMaxPlayersInConversation", &Script_BNGetMaxPlayersInConversation },
};

// OFFSET: 0x00530F60
void BattlenetUI_RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
