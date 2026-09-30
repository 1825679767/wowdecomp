#include "ClassTrainerFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: cea75209 auto
// OFFSET: 0x00593D90
static int32_t Script_GetTrainerGreetingText(lua_State* L) {
    lua_pushstring(L, CGClassTrainer::m_greetingText);
    return 1;
}

// VERIFY: 0f2603fe manual
// OFFSET: 0x00593D10
static int32_t Script_GetNumTrainerServices(lua_State* L) {
    lua_pushnumber(L, CGClassTrainer::m_filteredServices);
    return 1;
}

// VERIFY: ea1fc55f auto
// OFFSET: 0x00593D50
static int32_t Script_IsTradeskillTrainer(lua_State* L) {
    if (CGClassTrainer::m_trainerType == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: c40929fc manual
// OFFSET: 0x005943A0
static int32_t Script_SelectTrainerService(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectTrainerService(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGClassTrainer::m_numServices || (CGClassTrainer::m_currentSelection = CGClassTrainer::m_services.m_data[index]->m_spellID) < 0) {
        CGClassTrainer::m_currentSelection = 0;
    }

    return 0;
}

// VERIFY: 2aa7a31c auto
// OFFSET: 0x00594480
static int32_t Script_GetTrainerServiceCost(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceCost(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = index < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : nullptr;
    int32_t moneyCost = 0;
    int32_t talentCost;
    int32_t professionCost;

    if (service) {
        moneyCost = service->m_moneyCost;
        talentCost = service->m_talentCost;
        professionCost = service->m_professionCost;
    } else {
        talentCost = 0;
        professionCost = 0;
    }

    lua_pushnumber(L, moneyCost);
    lua_pushnumber(L, talentCost);
    lua_pushnumber(L, professionCost);
    return 3;
}

// VERIFY: bdbc7f4e auto
// OFFSET: 0x00594530
static int32_t Script_GetTrainerServiceLevelReq(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceLevelReq(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t levelReq = 0;

    if (index < CGClassTrainer::m_numServices) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[index];

        if (service) {
            levelReq = service->m_levelReq;
        }
    }

    lua_pushnumber(L, levelReq);
    return 1;
}

// VERIFY: fbed307c auto
// OFFSET: 0x005945B0
static int32_t Script_GetTrainerServiceNumAbilityReq(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceAbilityReq(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = index < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : nullptr;
    int32_t count = 0;

    if (service) {
        if (service->m_abilityReq[0] > 0) {
            count = 1;
        }

        if (service->m_abilityReq[1] > 0) {
            count++;
        }

        if (service->m_abilityReq[2] > 0) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// VERIFY: e8d6bc99 auto
// OFFSET: 0x00595B60
static int32_t Script_IsTrainerServiceSkillStep(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsTrainerServiceSkillStep(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGClassTrainer::m_numServices) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[index];

        if (service) {
            SpellRec spell;

            if (g_spellDB.GetRecord(service->m_spellID, &spell)) {
                for (uint32_t i = 0; i < 3; i++) {
                    if (spell.m_effect[i] == 44) {
                        lua_pushnumber(L, 1.0);
                        return 1;
                    }
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 48b1be31 auto
// OFFSET: 0x00594650
static int32_t Script_GetTrainerSkillLines(lua_State* L) {
    uint32_t count = CGClassTrainer::m_numSkillLines;

    for (uint32_t i = 0; i < count; i++) {
        int32_t skillLine = i < CGClassTrainer::m_numSkillLines ? CGClassTrainer::m_skillLines.m_data[i]->m_skillLine : 0;
        SkillLineRec* rec = g_skillLineDB.GetRecord(skillLine);
        lua_pushstring(L, rec ? rec->m_displayName : nullptr);
    }

    return count;
}

// VERIFY: fedc35c9 auto
// OFFSET: 0x00593EB0
static int32_t Script_GetTrainerSkillLineFilter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerSkillLineFilter(index)");
        return 0;
    }

    uint32_t filter = CGClassTrainer::m_skillLineFilter;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        for (uint32_t i = 0; i < CGClassTrainer::m_numSkillLines; i++) {
            if (!((1 << i) & filter)) {
                lua_pushnil(L);
                return 1;
            }
        }

        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (index >= CGClassTrainer::m_numSkillLines) {
        luaL_error(L, "Bad skill line in GetTrainerSkillLineFilter");
        return 0;
    }

    if ((1 << index) & filter) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 57f4fadd manual
// OFFSET: 0x005952F0
static int32_t Script_GetTrainerServiceSkillLine(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceSkillLine(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = index < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : nullptr;

    if (player && service) {
        SpellRec spell;

        if (g_spellDB.GetRecord(service->m_spellID, &spell)) {
            for (uint32_t i = 0; i < 3; i++) {
                int32_t effect = spell.m_effect[i];

                if (!effect) {
                    continue;
                }

                SkillLineRec* skill;

                if (effect == 44) {
                    skill = g_skillLineDB.GetRecord(spell.m_effectMiscValue[i]);
                } else {
                    int32_t spellId = effect != 36 || spell.m_implicitTargetA[i] == 5 ? spell.m_id : spell.m_effectTriggerSpell[i];
                    SkillLineAbilityRec* ability = SpellTableLookupAbility(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], spellId);

                    if (!ability) {
                        continue;
                    }

                    skill = g_skillLineDB.GetRecord(ability->m_skillLine);
                }

                if (skill) {
                    lua_pushstring(L, skill->m_displayName);
                    return 1;
                }
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00ACF3E8
static FrameScript_Method s_ScriptFunctions[] = {
    { "OpenTrainer", &Script_OpenTrainer },
    { "CloseTrainer", &Script_CloseTrainer },
    { "GetNumTrainerServices", &Script_GetNumTrainerServices },
    { "GetTrainerServiceInfo", &Script_GetTrainerServiceInfo },
    { "SelectTrainerService", &Script_SelectTrainerService },
    { "IsTradeskillTrainer", &Script_IsTradeskillTrainer },
    { "GetTrainerSelectionIndex", &Script_GetTrainerSelectionIndex },
    { "GetTrainerGreetingText", &Script_GetTrainerGreetingText },
    { "GetTrainerServiceIcon", &Script_GetTrainerServiceIcon },
    { "GetTrainerServiceSkillLine", &Script_GetTrainerServiceSkillLine },
    { "GetTrainerServiceCost", &Script_GetTrainerServiceCost },
    { "GetTrainerServiceLevelReq", &Script_GetTrainerServiceLevelReq },
    { "GetTrainerServiceSkillReq", &Script_GetTrainerServiceSkillReq },
    { "GetTrainerServiceNumAbilityReq", &Script_GetTrainerServiceNumAbilityReq },
    { "GetTrainerServiceAbilityReq", &Script_GetTrainerServiceAbilityReq },
    { "GetTrainerServiceStepReq", &Script_GetTrainerServiceStepReq },
    { "GetTrainerServiceDescription", &Script_GetTrainerServiceDescription },
    { "IsTrainerServiceSkillStep", &Script_IsTrainerServiceSkillStep },
    { "GetTrainerServiceStepIncrease", &Script_GetTrainerServiceStepIncrease },
    { "BuyTrainerService", &Script_BuyTrainerService },
    { "SetTrainerServiceTypeFilter", &Script_SetTrainerServiceTypeFilter },
    { "SetTrainerSkillLineFilter", &Script_SetTrainerSkillLineFilter },
    { "GetTrainerServiceTypeFilter", &Script_GetTrainerServiceTypeFilter },
    { "GetTrainerSkillLineFilter", &Script_GetTrainerSkillLineFilter },
    { "GetTrainerSkillLines", &Script_GetTrainerSkillLines },
    { "CollapseTrainerSkillLine", &Script_CollapseTrainerSkillLine },
    { "ExpandTrainerSkillLine", &Script_ExpandTrainerSkillLine },
    { "GetTrainerServiceItemLink", &Script_GetTrainerServiceItemLink },
};

// VERIFY: 9493cc0a auto
// OFFSET: 0x00593F70
void ClassTrainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
