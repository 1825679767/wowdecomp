#include "RealmList.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"

int32_t Script_RequestRealmList(lua_State* L);
int32_t Script_RealmListUpdateRate(lua_State* L);
int32_t Script_CancelRealmListQuery(lua_State* L);
int32_t Script_GetNumRealms(lua_State* L);
int32_t Script_GetRealmInfo(lua_State* L);
int32_t Script_ChangeRealm(lua_State* L);
int32_t Script_GetRealmCategories(lua_State* L);
int32_t Script_SetPreferredInfo(lua_State* L);
int32_t Script_SortRealms(lua_State* L);
int32_t Script_GetSelectedCategory(lua_State* L);
int32_t Script_RealmListDialogCancelled(lua_State* L);
int32_t Script_IsInvalidTournamentRealmCategory(lua_State* L);
int32_t Script_IsTournamentRealmCategory(lua_State* L);
int32_t Script_IsInvalidLocale(lua_State* L);

// STATIC: 0x00AC4190
static FrameScript_Method s_scriptFunctions[] = {
    { "RequestRealmList", &Script_RequestRealmList },
    { "RealmListUpdateRate", &Script_RealmListUpdateRate },
    { "CancelRealmListQuery", &Script_CancelRealmListQuery },
    { "GetNumRealms", &Script_GetNumRealms },
    { "GetRealmInfo", &Script_GetRealmInfo },
    { "ChangeRealm", &Script_ChangeRealm },
    { "GetRealmCategories", &Script_GetRealmCategories },
    { "SetPreferredInfo", &Script_SetPreferredInfo },
    { "SortRealms", &Script_SortRealms },
    { "GetSelectedCategory", &Script_GetSelectedCategory },
    { "RealmListDialogCancelled", &Script_RealmListDialogCancelled },
    { "IsInvalidTournamentRealmCategory", &Script_IsInvalidTournamentRealmCategory },
    { "IsTournamentRealmCategory", &Script_IsTournamentRealmCategory },
    { "IsInvalidLocale", &Script_IsInvalidLocale },
};

// VERIFY: 98b99f13 auto
// OFFSET: 0x004DE4D0
void RealmListRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}
