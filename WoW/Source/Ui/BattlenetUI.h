#ifndef WOW_SOURCE_UI_BATTLENETUI_H
#define WOW_SOURCE_UI_BATTLENETUI_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class Login;
struct lua_State;

struct BattlenetPresenceValue {
    /* 0000 */ int32_t m_type;
    /* 0004 */ uint8_t m_bool;
    /* 0005 */ uint8_t unk_0005[519];
};

struct BattlenetBlockList {
    /* 0000 */ uint32_t m_blockedCount;
    /* 0004 */ uint32_t m_blocked[100];
    /* 0194 */ uint32_t m_blockedToonCount;
    /* 0198 */ uint32_t m_blockedToons[100];
};

struct BattlenetUIFriend {
    /* 0000 */ uint32_t m_presenceID;
    /* 0004 */ uint8_t unk_0004[820];
};

struct BattlenetInviteList {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_count;
};

class BattlenetUI {
    public:
    static BattlenetInviteList* m_friendInvites;
    static int32_t unk_BD1B00;
    static uint8_t m_friendListLoaded;
    static TSGrowableArray<BattlenetUIFriend> m_friends;

    static void EnterRealm(const char* nameFormat);
    static void PopulateAutoComplete();

    /* 0000 */ uint8_t unk_0000 [4];
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t unk_000C;
};

namespace BattlenetAPI {
    bool IsRIDEnabled();
    BattlenetPresenceValue GetPresenceValue(uint32_t presenceID, uint32_t field);
    BattlenetBlockList GetBlockList();
}

bool BattlenetUI_IsAvailable();
void BattlenetUI_EnterRealm();
void BattlenetUI_LeaveRealm();

int32_t Script_BNGetInfo(lua_State* L);
int32_t Script_BNGetFriendInfo(lua_State* L);
int32_t Script_BNGetFriendInfoByID(lua_State* L);
int32_t Script_BNGetNumFriendToons(lua_State* L);
int32_t Script_BNGetFriendToonInfo(lua_State* L);
int32_t Script_BNGetToonInfo(lua_State* L);
int32_t Script_BNRemoveFriend(lua_State* L);
int32_t Script_BNSetFriendNote(lua_State* L);
int32_t Script_BNSetSelectedFriend(lua_State* L);
int32_t Script_BNGetSelectedFriend(lua_State* L);
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
int32_t Script_BNGetBlockedInfo(lua_State* L);
int32_t Script_BNIsBlocked(lua_State* L);
int32_t Script_BNSetBlocked(lua_State* L);
int32_t Script_BNSetSelectedBlock(lua_State* L);
int32_t Script_BNGetSelectedBlock(lua_State* L);
int32_t Script_BNGetBlockedToonInfo(lua_State* L);
int32_t Script_BNIsToonBlocked(lua_State* L);
int32_t Script_BNSetToonBlocked(lua_State* L);
int32_t Script_BNSetSelectedToonBlock(lua_State* L);
int32_t Script_BNGetSelectedToonBlock(lua_State* L);
int32_t Script_BNReportPlayer(lua_State* L);
int32_t Script_BNConnected(lua_State* L);
int32_t Script_IsBNLogin(lua_State* L);
int32_t Script_BNRequestFOFInfo(lua_State* L);
int32_t Script_BNGetNumFOF(lua_State* L);
int32_t Script_BNGetFOFInfo(lua_State* L);
int32_t Script_BNSetMatureLanguageFilter(lua_State* L);
int32_t Script_BNGetMatureLanguageFilter(lua_State* L);
int32_t Script_BNIsSelf(lua_State* L);
int32_t Script_BNIsFriend(lua_State* L);
void BattlenetUI_RegisterScriptFunctions();
void BattlenetUI_UnregisterScriptFunctions();

#endif
