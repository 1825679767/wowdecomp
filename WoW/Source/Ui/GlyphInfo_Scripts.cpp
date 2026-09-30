#include "GlyphInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: 6144c30d auto
// OFFSET: 0x005B71E0
static int32_t Script_GetNumGlyphSockets(lua_State* L) {
    lua_pushnumber(L, 6.0);
    return 1;
}

// VERIFY: 2a5b7041 manual
// OFFSET: 0x005B7660
static int32_t Script_RemoveGlyphFromSocket(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: RemoveGlyphFromSocket(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index < 6 && player && player->m_player->m_playerFieldGlyphSlots1[index]) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_REMOVE_GLYPH));
        msg.Put(index);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// STATIC: 0x00ACFF54
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumGlyphSockets", &Script_GetNumGlyphSockets },
    { "GetGlyphSocketInfo", &Script_GetGlyphSocketInfo },
    { "GlyphMatchesSocket", &Script_GlyphMatchesSocket },
    { "PlaceGlyphInSocket", &Script_PlaceGlyphInSocket },
    { "RemoveGlyphFromSocket", &Script_RemoveGlyphFromSocket },
    { "GetGlyphLink", &Script_GetGlyphLink },
};

// VERIFY: 177745e8 auto
// OFFSET: 0x005B7210
void GlyphInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
