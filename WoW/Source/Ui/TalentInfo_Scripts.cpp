#include "TalentInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"

// VERIFY: 3e3b2c80 auto
// OFFSET: 0x005C5810
static int32_t Script_GetActiveTalentGroup(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group;

    if (inspect) {
        group = CGTalentInfo_Inspect::m_activeTalentGroup;
    } else {
        group = pet ? 0 : CGTalentInfo::m_activeTalentGroup;
    }

    lua_pushnumber(L, group + 1);
    return 1;
}

// VERIFY: 2c14e75a auto
// OFFSET: 0x005C63B0
static int32_t Script_GetPreviewTalentPointsSpent(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPreviewTalentPointsSpent(tabIndex[, isPet[, groupIndex]])");
    }

    lua_tointeger(L, 1);
    StringToBOOL(L, 2, 0);

    if (lua_isnumber(L, 3)) {
        lua_tointeger(L, 3);
    }

    lua_pushinteger(L, 0);
    return 1;
}

// STATIC: 0x00AD05F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumTalentTabs", &Script_GetNumTalentTabs },
    { "GetTalentTabInfo", &Script_GetTalentTabInfo },
    { "GetNumTalents", &Script_GetNumTalents },
    { "GetTalentInfo", &Script_GetTalentInfo },
    { "GetTalentLink", &Script_GetTalentLink },
    { "GetTalentPrereqs", &Script_GetTalentPrereqs },
    { "LearnTalent", &Script_LearnTalent },
    { "GetUnspentTalentPoints", &Script_GetUnspentTalentPoints },
    { "GetNumTalentGroups", &Script_GetNumTalentGroups },
    { "GetActiveTalentGroup", &Script_GetActiveTalentGroup },
    { "SetActiveTalentGroup", &Script_SetActiveTalentGroup },
    { "GetPreviewTalentPointsSpent", &Script_GetPreviewTalentPointsSpent },
    { "GetGroupPreviewTalentPointsSpent", &Script_GetGroupPreviewTalentPointsSpent },
    { "AddPreviewTalentPoints", &Script_AddPreviewTalentPoints },
    { "ResetPreviewTalentPoints", &Script_ResetPreviewTalentPoints },
    { "ResetGroupPreviewTalentPoints", &Script_ResetGroupPreviewTalentPoints },
    { "LearnPreviewTalents", &Script_LearnPreviewTalents },
};

// VERIFY: c3db1349 auto
// OFFSET: 0x005C5880
void TalentInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
