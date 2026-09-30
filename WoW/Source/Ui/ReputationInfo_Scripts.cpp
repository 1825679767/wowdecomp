#include "ReputationInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: 1e40863f auto
// OFFSET: 0x005D1FC0
static int32_t Script_CollapseAllFactionHeaders(lua_State* L) {
    CGReputationInfo::CollapseHeader(CGReputationInfo::m_numFactions, 1);
    return 0;
}

// VERIFY: 6b33faf0 auto
// OFFSET: 0x005D1FE0
static int32_t Script_ExpandAllFactionHeaders(lua_State* L) {
    CGReputationInfo::CollapseHeader(CGReputationInfo::m_numFactions, 0);
    return 0;
}

// VERIFY: 26793d47 manual
// OFFSET: 0x005CFF20
static int32_t Script_GetNumFactions(lua_State* L) {
    lua_pushnumber(L, CGReputationInfo::m_numShownFactions);
    return 1;
}

// VERIFY: 06bc7cb4 auto
// OFFSET: 0x005D1F70
static int32_t Script_CollapseFactionHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseFactionHeader(index)");
    }

    CGReputationInfo::CollapseHeader(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, 1);
    return 0;
}

// VERIFY: fc0d4302 auto
// OFFSET: 0x005D2000
static int32_t Script_ExpandFactionHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandFactionHeader(index)");
    }

    CGReputationInfo::CollapseHeader(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, 0);
    return 0;
}

// VERIFY: 0666d394 auto
// OFFSET: 0x005D0740
static int32_t Script_IsFactionInactive(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsFactionInactive(index)");
    }

    if (CGReputationInfo::IsFactionInactive(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 6260777e auto
// OFFSET: 0x005D07B0
static int32_t Script_SetSelectedFaction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedFaction(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index <= CGReputationInfo::m_numFactions) {
        CGReputationInfo::m_selectedFaction = CGReputationInfo::m_factionList.m_data[index]->m_factionId;
    } else {
        CGReputationInfo::m_selectedFaction = 0;
    }

    return 0;
}

// VERIFY: 941cdb0b manual
// OFFSET: 0x005D1240
static int32_t Script_GetWatchedFactionInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    FactionRec* rec = nullptr;
    int32_t faction = 0;

    if (player) {
        uint32_t index = player->m_player->m_playerFieldWatchedFactionIndex;
        faction = index > 127 ? 0 : CGReputationInfo::m_factionStandings[index].m_factionId;
        rec = g_factionDB.GetRecord(faction);
    }

    if (!rec) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 5;
    }

    lua_pushstring(L, rec->m_name);
    int32_t reaction = CGReputationInfo::GetFactionStandingReaction(faction);
    lua_pushnumber(L, reaction + 1);
    int32_t max = CGReputationInfo::m_reactionThresholds[reaction + 1];
    int32_t value = GetRepListRepValue(faction);
    lua_pushnumber(L, CGReputationInfo::m_reactionThresholds[reaction]);
    lua_pushnumber(L, max);
    lua_pushnumber(L, value);
    return 5;
}

// STATIC: 0x00AD0AE8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFactions", &Script_GetNumFactions },
    { "GetFactionInfo", &Script_GetFactionInfo },
    { "GetFactionInfoByID", &Script_GetFactionInfoByID },
    { "GetWatchedFactionInfo", &Script_GetWatchedFactionInfo },
    { "SetWatchedFactionIndex", &Script_SetWatchedFactionIndex },
    { "FactionToggleAtWar", &Script_FactionToggleAtWar },
    { "CollapseFactionHeader", &Script_CollapseFactionHeader },
    { "CollapseAllFactionHeaders", &Script_CollapseAllFactionHeaders },
    { "SetFactionInactive", &Script_SetFactionInactive },
    { "SetFactionActive", &Script_SetFactionActive },
    { "IsFactionInactive", &Script_IsFactionInactive },
    { "ExpandFactionHeader", &Script_ExpandFactionHeader },
    { "ExpandAllFactionHeaders", &Script_ExpandAllFactionHeaders },
    { "SetSelectedFaction", &Script_SetSelectedFaction },
    { "GetSelectedFaction", &Script_GetSelectedFaction },
};

// VERIFY: 5ad3f6ee auto
// OFFSET: 0x005CFF60
void ReputationInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
