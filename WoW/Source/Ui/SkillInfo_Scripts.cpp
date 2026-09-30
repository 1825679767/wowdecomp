#include "SkillInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: b1ab49e5 manual
// OFFSET: 0x005CD1B0
static int32_t Script_GetNumSkillLines(lua_State* L) {
    lua_pushnumber(L, CGSkillInfo::m_numSkillLines);
    return 1;
}

// VERIFY: 6d37f79d manual
// OFFSET: 0x005CD860
static int32_t Script_SetSelectedSkill(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedSkill(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGSkillInfo::m_selectedSkill = index >= CGSkillInfo::m_numLines ? 0 : CGSkillInfo::m_lines.m_data[index]->m_skillLineID;
    return 0;
}

// VERIFY: fa236c5d auto
// OFFSET: 0x005CD8F0
static int32_t Script_GetSelectedSkill(lua_State* L) {
    lua_pushnumber(L, CGSkillInfo::GetSelectedIndex() + 1);
    return 1;
}

// VERIFY: 7eaea3f3 manual
// OFFSET: 0x005CE540
static int32_t Script_GetAdjustedSkillPoints(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t points = 0;

    if (player) {
        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            points = player->m_player->m_playerCharacterPoints[1] - CGSkillInfo::m_skillUpPoints;
        } else {
            points = -CGSkillInfo::m_skillUpPoints;
        }
    }

    lua_pushnumber(L, points);
    return 1;
}

// STATIC: 0x00AD09B8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumSkillLines", &Script_GetNumSkillLines },
    { "GetSkillLineInfo", &Script_GetSkillLineInfo },
    { "AbandonSkill", &Script_AbandonSkill },
    { "CollapseSkillHeader", &Script_CollapseSkillHeader },
    { "ExpandSkillHeader", &Script_ExpandSkillHeader },
    { "AddSkillUp", &Script_AddSkillUp },
    { "RemoveSkillUp", &Script_RemoveSkillUp },
    { "GetAdjustedSkillPoints", &Script_GetAdjustedSkillPoints },
    { "AcceptSkillUps", &Script_AcceptSkillUps },
    { "CancelSkillUps", &Script_CancelSkillUps },
    { "BuySkillTier", &Script_BuySkillTier },
    { "SetSelectedSkill", &Script_SetSelectedSkill },
    { "GetSelectedSkill", &Script_GetSelectedSkill },
};

// VERIFY: 8f357cd9 auto
// OFFSET: 0x005CD1F0
void SkillInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
