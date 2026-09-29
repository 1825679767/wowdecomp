#include "FriendList.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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
