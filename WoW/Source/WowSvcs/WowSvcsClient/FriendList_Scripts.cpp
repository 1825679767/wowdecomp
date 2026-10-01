#include "FriendList.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C7BFEC
static int32_t s_whoToUI;

// VERIFY: cec9da47 auto
// OFFSET: 0x006B3430
static int32_t Script_SetWhoToUI(lua_State* L) {
    s_whoToUI = StringToBOOL(L, 1, 0);
    return 0;
}

// VERIFY: 7237b97d manual
// OFFSET: 0x006B33D0
static int32_t Script_GetNumWhoResults(lua_State* L) {
    lua_pushnumber(L, FriendList::s_numWhoResults);
    lua_pushnumber(L, FriendList::s_totalWhoResults);
    return 2;
}

// VERIFY: 36c3cb43 manual
// OFFSET: 0x006B44F0
static int32_t Script_ShowFriends(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CONTACT_LIST));
    msg.Put(static_cast<uint32_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// VERIFY: dcb94d85 manual
// OFFSET: 0x006B4060
static int32_t Script_GetNumFriends(lua_State* L) {
    FriendList* list = FriendList::s_friendList;

    if (!list) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    lua_pushnumber(L, list->GetNumFriends());
    uint32_t online = 0;
    uint32_t count = list->GetNumFriends();

    for (uint32_t i = 0; i < count; i++) {
        if (i <= 100 && list->m_friends[i].m_connected) {
            online++;
        }
    }

    lua_pushnumber(L, online);
    return 2;
}

// VERIFY: 1ace5852 manual
// OFFSET: 0x006B4560
static int32_t Script_GetNumIgnores(lua_State* L) {
    if (FriendList::s_friendList) {
        lua_pushnumber(L, FriendList::s_friendList->GetNumIgnores());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// STATIC: 0x00AD93D8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFriends", &Script_GetNumFriends },
    { "GetFriendInfo", &Script_GetFriendInfo },
    { "SetSelectedFriend", &Script_SetSelectedFriend },
    { "GetSelectedFriend", &Script_GetSelectedFriend },
    { "AddOrRemoveFriend", &Script_AddOrRemoveFriend },
    { "AddFriend", &Script_AddFriend },
    { "RemoveFriend", &Script_RemoveFriend },
    { "ShowFriends", &Script_ShowFriends },
    { "SetFriendNotes", &Script_SetFriendNotes },
    { "GetNumIgnores", &Script_GetNumIgnores },
    { "GetIgnoreName", &Script_GetIgnoreName },
    { "SetSelectedIgnore", &Script_SetSelectedIgnore },
    { "GetSelectedIgnore", &Script_GetSelectedIgnore },
    { "AddOrDelIgnore", &Script_AddOrDelIgnore },
    { "AddIgnore", &Script_AddIgnore },
    { "DelIgnore", &Script_DelIgnore },
    { "GetNumMutes", &Script_GetNumMutes },
    { "GetMuteName", &Script_GetMuteName },
    { "SetSelectedMute", &Script_SetSelectedMute },
    { "GetSelectedMute", &Script_GetSelectedMute },
    { "AddOrDelMute", &Script_AddOrDelMute },
    { "AddMute", &Script_AddMute },
    { "DelMute", &Script_DelMute },
    { "IsIgnored", &Script_IsIgnored },
    { "IsMuted", &Script_IsMuted },
    { "IsIgnoredOrMuted", &Script_IsIgnoredOrMuted },
    { "SendWho", &Script_SendWho },
    { "GetNumWhoResults", &Script_GetNumWhoResults },
    { "GetWhoInfo", &Script_GetWhoInfo },
    { "SetWhoToUI", &Script_SetWhoToUI },
    { "SortWho", &Script_SortWho },
};

// VERIFY: 662b6abe auto
// OFFSET: 0x006B3450
void FriendList::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
