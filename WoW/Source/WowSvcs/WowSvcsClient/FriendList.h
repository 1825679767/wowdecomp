#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FRIENDLIST_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FRIENDLIST_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct FriendInfo {
    /* 0000 */ uint8_t m_status;
    /* 0001 */ uint8_t m_flags;
    /* 0002 */ uint8_t unk_0002[518];
    /* 0208 */ WOWGUID m_guid;
    /* 0210 */ uint8_t unk_0210[16];
};

class FriendList {
    public:
    static FriendList* s_friendList;

    /* 0000 */ FriendInfo m_friends[100];

    FriendInfo* GetFriend(WOWGUID guid);
    uint32_t GetNumFriends();

    static void Destroy();
    static void Initialize();
    static void RegisterScriptFunctions();

    bool IsIgnored(WOWGUID guid, bool a2);
    bool IsFriend(WOWGUID guid);
};

int32_t Script_GetFriendInfo(lua_State* L);
int32_t Script_SetSelectedFriend(lua_State* L);
int32_t Script_GetSelectedFriend(lua_State* L);
int32_t Script_AddOrRemoveFriend(lua_State* L);
int32_t Script_AddFriend(lua_State* L);
int32_t Script_RemoveFriend(lua_State* L);
int32_t Script_SetFriendNotes(lua_State* L);
int32_t Script_GetNumIgnores(lua_State* L);
int32_t Script_GetIgnoreName(lua_State* L);
int32_t Script_SetSelectedIgnore(lua_State* L);
int32_t Script_GetSelectedIgnore(lua_State* L);
int32_t Script_AddOrDelIgnore(lua_State* L);
int32_t Script_AddIgnore(lua_State* L);
int32_t Script_DelIgnore(lua_State* L);
int32_t Script_GetNumMutes(lua_State* L);
int32_t Script_GetMuteName(lua_State* L);
int32_t Script_SetSelectedMute(lua_State* L);
int32_t Script_GetSelectedMute(lua_State* L);
int32_t Script_AddOrDelMute(lua_State* L);
int32_t Script_AddMute(lua_State* L);
int32_t Script_DelMute(lua_State* L);
int32_t Script_IsIgnored(lua_State* L);
int32_t Script_IsMuted(lua_State* L);
int32_t Script_IsIgnoredOrMuted(lua_State* L);
int32_t Script_SendWho(lua_State* L);
int32_t Script_GetWhoInfo(lua_State* L);
int32_t Script_SortWho(lua_State* L);

#endif
