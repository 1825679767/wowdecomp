#include "BattlenetUI.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
