#include "GossipInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

// STATIC: 0x00ACEF20
static const char* s_gossipTypes[22] = {
    "gossip",
    "vendor",
    "taxi",
    "trainer",
    "healer",
    "binder",
    "banker",
    "petition",
    "tabard",
    "battlemaster",
    "auctioneer",
    "gossip",
    "gossip",
    "gossip",
    nullptr,
    nullptr,
    "gossip",
    "gossip",
    "gossip",
    "gossip",
    "gossip",
    nullptr,
};

// VERIFY: e97b7e00 manual
// OFFSET: 0x0058A920
static int32_t Script_GetNumGossipOptions(lua_State* L) {
    lua_pushnumber(L, CGGossipInfo::m_gossipOptionsCount);
    return 1;
}

// VERIFY: 2aa162f8 manual
// OFFSET: 0x0058AB50
static int32_t Script_ForceGossip(lua_State* L) {
    bool force = false;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGossipInfo::m_gossip, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        force = unit->IsForceGossip();
    }

    lua_pushboolean(L, force);
    return 1;
}

// VERIFY: bd0c95ec auto
// OFFSET: 0x0058A900
static int32_t Script_GetGossipText(lua_State* L) {
    lua_pushstring(L, CGGossipInfo::m_gossipText);
    return 1;
}

// VERIFY: 3429fd49 auto
// OFFSET: 0x0058A9E0
static int32_t Script_GetGossipOptions(lua_State* L) {
    int32_t count = 0;

    for (GossipOption* option = CGGossipInfo::m_options; option < reinterpret_cast<GossipOption*>(CGGossipInfo::m_gossipText); option++) {
        if (option && option->m_index >= 0) {
            lua_checkstack(L, 2);
            lua_pushstring(L, option->m_text);
            lua_pushstring(L, s_gossipTypes[option->m_type]);
            count++;
        }
    }

    return 2 * count;
}

// STATIC: 0x00ACEF78
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetGossipText", &Script_GetGossipText },
    { "GetNumGossipOptions", &Script_GetNumGossipOptions },
    { "GetNumGossipAvailableQuests", &Script_GetNumGossipAvailableQuests },
    { "GetNumGossipActiveQuests", &Script_GetNumGossipActiveQuests },
    { "GetGossipOptions", &Script_GetGossipOptions },
    { "GetGossipAvailableQuests", &Script_GetGossipAvailableQuests },
    { "GetGossipActiveQuests", &Script_GetGossipActiveQuests },
    { "SelectGossipOption", &Script_SelectGossipOption },
    { "SelectGossipAvailableQuest", &Script_SelectGossipAvailableQuest },
    { "SelectGossipActiveQuest", &Script_SelectGossipActiveQuest },
    { "CloseGossip", &Script_CloseGossip },
    { "ForceGossip", &Script_ForceGossip },
};

// VERIFY: 872f7321 auto
// OFFSET: 0x0058AA60
void GossipInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
