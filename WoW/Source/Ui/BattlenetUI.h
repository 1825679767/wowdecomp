#ifndef WOW_SOURCE_UI_BATTLENETUI_H
#define WOW_SOURCE_UI_BATTLENETUI_H

#include <cstddef>
#include <cstdint>

struct lua_State;

class BattlenetUI {
    public:
    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
};

void BattlenetUI_EnterRealm();
void BattlenetUI_LeaveRealm();

int32_t Script_BNGetInfo(lua_State* L);
int32_t Script_BNGetNumFriends(lua_State* L);
int32_t Script_BNGetFriendInfo(lua_State* L);
int32_t Script_BNGetFriendInfoByID(lua_State* L);
int32_t Script_BNGetNumFriendToons(lua_State* L);
int32_t Script_BNGetFriendToonInfo(lua_State* L);
int32_t Script_BNGetToonInfo(lua_State* L);
int32_t Script_BNRemoveFriend(lua_State* L);
int32_t Script_BNSetFriendNote(lua_State* L);
int32_t Script_BNSetSelectedFriend(lua_State* L);
int32_t Script_BNGetSelectedFriend(lua_State* L);
int32_t Script_BNGetNumFriendInvites(lua_State* L);
int32_t Script_BNGetFriendInviteInfo(lua_State* L);
int32_t Script_BNSendFriendInvite(lua_State* L);
int32_t Script_BNSendFriendInviteByID(lua_State* L);
int32_t Script_BNAcceptFriendInvite(lua_State* L);
int32_t Script_BNDeclineFriendInvite(lua_State* L);
int32_t Script_BNReportFriendInvite(lua_State* L);
int32_t Script_BNSetAFK(lua_State* L);
int32_t Script_BNSetDND(lua_State* L);
int32_t Script_BNSetCustomMessage(lua_State* L);
int32_t Script_BNGetCustomMessageTable(lua_State* L);
int32_t Script_BNSetFocus(lua_State* L);
int32_t Script_BNSendWhisper(lua_State* L);
int32_t Script_BNCreateConversation(lua_State* L);
int32_t Script_BNInviteToConversation(lua_State* L);
int32_t Script_BNLeaveConversation(lua_State* L);
int32_t Script_BNSendConversationMessage(lua_State* L);
int32_t Script_BNGetNumConversationMembers(lua_State* L);
int32_t Script_BNGetConversationMemberInfo(lua_State* L);
int32_t Script_BNGetConversationInfo(lua_State* L);
int32_t Script_BNListConversation(lua_State* L);
int32_t Script_BNGetNumBlocked(lua_State* L);
int32_t Script_BNGetBlockedInfo(lua_State* L);
int32_t Script_BNIsBlocked(lua_State* L);
int32_t Script_BNSetBlocked(lua_State* L);
int32_t Script_BNSetSelectedBlock(lua_State* L);
int32_t Script_BNGetSelectedBlock(lua_State* L);
int32_t Script_BNGetNumBlockedToons(lua_State* L);
int32_t Script_BNGetBlockedToonInfo(lua_State* L);
int32_t Script_BNIsToonBlocked(lua_State* L);
int32_t Script_BNSetToonBlocked(lua_State* L);
int32_t Script_BNSetSelectedToonBlock(lua_State* L);
int32_t Script_BNGetSelectedToonBlock(lua_State* L);
int32_t Script_BNReportPlayer(lua_State* L);
int32_t Script_BNConnected(lua_State* L);
int32_t Script_BNFeaturesEnabledAndConnected(lua_State* L);
int32_t Script_IsBNLogin(lua_State* L);
int32_t Script_BNFeaturesEnabled(lua_State* L);
int32_t Script_BNRequestFOFInfo(lua_State* L);
int32_t Script_BNGetNumFOF(lua_State* L);
int32_t Script_BNGetFOFInfo(lua_State* L);
int32_t Script_BNSetMatureLanguageFilter(lua_State* L);
int32_t Script_BNGetMatureLanguageFilter(lua_State* L);
int32_t Script_BNIsSelf(lua_State* L);
int32_t Script_BNIsFriend(lua_State* L);
int32_t Script_BNGetMaxPlayersInConversation(lua_State* L);
void BattlenetUI_RegisterScriptFunctions();

#endif
