#include "PetInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/ActionBarFrame.h"

// VERIFY: a26fb522 manual
// OFFSET: 0x005D4EC0
static int32_t Script_GetPetActionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPetActionInfo(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t* action = CGPetInfo::GetPet(0) && index < sizeof(CGPetInfo::m_actions) / sizeof(CGPetInfo::m_actions[0]) ? &CGPetInfo::m_actions[index] : nullptr;

    if (!action || !*action) {
        for (uint32_t i = 0; i < 7; i++) {
            lua_pushnil(L);
        }

        return 7;
    }

    uint32_t id = *action & 0xFFFFFF;
    bool autoCastEnabled = false;
    char buffer[64];

    switch ((*action >> 24) & 0x3F) {
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {
            SpellRec spell;
            int32_t found = g_spellDB.GetRecord(id, &spell);

            if (found) {
                lua_pushstring(L, spell.m_name);
                lua_pushstring(L, spell.m_nameSubtext);
                CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
                SpellIconRec* icon = g_spellIconDB.GetRecord(Spell_C_IsActiveSpell(&spell, pet) && spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

                if (icon) {
                    lua_pushstring(L, icon->m_textureFilename);
                } else {
                    lua_pushnil(L);
                }
            } else {
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
            }

            lua_pushnil(L);
            lua_pushnil(L);
            bool autoCastAllowed = found && (*action & 0x80000000);
            autoCastEnabled = found && (*action & 0x40000000);

            if (autoCastAllowed) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnil(L);
            }

            break;
        }
        case 6: {
            SStrPrintf(buffer, sizeof(buffer), "PET_MODE_%s", CGPetInfo::GetModeToken(id));
            lua_pushstring(L, buffer);
            lua_pushnil(L);
            SStrPrintf(buffer, sizeof(buffer), "PET_%s_TEXTURE", CGPetInfo::GetModeToken(id));
            lua_pushstring(L, buffer);
            lua_pushnumber(L, 1.0);

            if (CGPetInfo::GetPetMode() == id) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnil(L);
            }

            lua_pushnil(L);
            break;
        }
        case 7: {
            SStrPrintf(buffer, sizeof(buffer), "PET_ACTION_%s", CGPetInfo::GetOrdersToken(id));
            lua_pushstring(L, buffer);
            lua_pushnil(L);
            SStrPrintf(buffer, sizeof(buffer), "PET_%s_TEXTURE", CGPetInfo::GetOrdersToken(id));
            lua_pushstring(L, buffer);
            lua_pushnumber(L, 1.0);

            if (id == 2 ? CGPetInfo::m_attackingTarget != 0 : (CGPetInfo::m_petMode >> 8) == id) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnil(L);
            }

            lua_pushnil(L);
            break;
        }
        default:
            lua_pushnil(L);
            break;
    }

    if (autoCastEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 7;
}

// VERIFY: ab3a26c1 auto
// OFFSET: 0x005D5640
static int32_t Script_PetStopAttack(lua_State* L) {
    CGPetInfo::PetClearTarget();
    return 0;
}

// VERIFY: 6841a93e auto
// OFFSET: 0x005D3160
static int32_t Script_IsPetAttackActive(lua_State* L) {
    lua_pushboolean(L, CGPetInfo::m_attackingTarget);
    return 1;
}

// VERIFY: c4e057ce manual
// OFFSET: 0x005D3180
static int32_t Script_GetPetTimeRemaining(lua_State* L) {
    uint32_t expiration = CGPetInfo::m_expirationTime;

    if (!expiration) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t remaining = expiration == OsGetAsyncTimeMs() ? 0 : expiration - OsGetAsyncTimeMs();
    lua_pushnumber(L, remaining);
    return 1;
}

// VERIFY: cab5e1fd manual
// OFFSET: 0x005D3780
static int32_t Script_PetCanBeAbandoned(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer() && (pet->m_unit->m_unitFieldBytes2[2] & 2)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2e7b5ffe manual
// OFFSET: 0x005D3820
static int32_t Script_PetCanBeDismissed(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    SummonPropertiesRec* properties;

    if (pet && pet->m_unit->m_unitFieldCreatedby == ClntObjMgrGetActivePlayer() && (properties = pet->GetSummonProperties()) && (properties->m_flags & 0x20)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: a98e1721 manual
// OFFSET: 0x005D38C0
static int32_t Script_PetCanBeRenamed(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer() && (pet->m_unit->m_unitFieldBytes2[2] & 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: d37eccba manual
// OFFSET: 0x005D3CE0
static int32_t Script_GetPetIcon(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    CreatureFamilyRec* family;

    if (pet && pet->CanGainXP() && (family = g_creatureFamilyDB.GetRecord(pet->GetCreatureFamily()))) {
        lua_pushstring(L, family->m_iconFile);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 80fd0a13 manual
// OFFSET: 0x005D3960
static int32_t Script_HasPetUI(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && !(pet->m_obj->m_type & TYPEMASK_PLAYER) && pet->m_unit->m_unitFieldPetnumber) {
        lua_pushnumber(L, 1.0);

        if (pet->CanGainXP()) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// VERIFY: 4ac52271 manual
// OFFSET: 0x005D3A20
static int32_t Script_GetPetExperience(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->CanGainXP()) {
        lua_pushnumber(L, pet->m_unit->m_unitFieldPetexperience);
        lua_pushnumber(L, pet->m_unit->m_unitFieldPetnextlevelexp);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: e7c0a80b manual
// OFFSET: 0x005D5A10
static int32_t Script_IsPetAttackAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsPetAttackAction(index)");
        return 0;
    }

    lua_pushboolean(L, CGPetInfo::IsAttackAction(lua_tointeger(L, 1) - 1));
    return 1;
}

// VERIFY: e5c8a9a7 manual
// OFFSET: 0x005D5280
static int32_t Script_GetPetActionCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPetActionCooldown(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t* action = CGPetInfo::GetPet(0) && index < sizeof(CGPetInfo::m_actions) / sizeof(CGPetInfo::m_actions[0]) ? &CGPetInfo::m_actions[index] : nullptr;
    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;

    if (action && *action) {
        uint32_t type = (*action >> 24) & 0x3F;

        if (type == 1 || type - 8 <= 9) {
            Spell_C_GetSpellCooldown(*action & 0xFFFFFF, 1, &duration, &start, &enable);
        }
    }

    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// VERIFY: 716497e3 auto
// OFFSET: 0x005D3720
static int32_t Script_PetHasActionBar(lua_State* L) {
    if (s_pets.Count() && s_pets.m_data[0] && !(CGActionBar::m_tempPageActiveFlags & 1) && CGPetInfo::IsPetBarUsed()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 7527b202 manual
// OFFSET: 0x005D5820
static int32_t Script_GetPossessInfo(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: GetPossessInfo(index)");
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && CGPetInfo::m_possessSpell) {
        int64_t index = static_cast<int64_t>(lua_tonumber(L, 1));

        if (index == 1) {
            SpellRec spell;

            if (g_spellDB.GetRecord(CGPetInfo::m_possessSpell, &spell)) {
                SpellIconRec* icon = g_spellIconDB.GetRecord(spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

                if (icon) {
                    lua_pushstring(L, icon->m_textureFilename);
                } else {
                    lua_pushnil(L);
                }

                lua_pushstring(L, spell.m_name);
                lua_pushboolean(L, 1);
                return 3;
            }
        } else if (index == 2) {
            SpellIconRec* icon = g_spellIconDB.GetRecord(693);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushnil(L);
            }

            int32_t enabled = 1;
            SpellRec spell;

            if (g_spellDB.GetRecord(CGPetInfo::m_possessSpell, &spell) && spell.m_attributes < 0) {
                enabled = 0;
            }

            lua_pushstring(L, spell.m_name);
            lua_pushboolean(L, enabled);
            return 3;
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// STATIC: 0x00AD0C30
static FrameScript_Method s_ScriptFunctions[] = {
    { "PetHasActionBar", &Script_PetHasActionBar },
    { "GetPetActionInfo", &Script_GetPetActionInfo },
    { "GetPetActionCooldown", &Script_GetPetActionCooldown },
    { "GetPetActionsUsable", &Script_GetPetActionsUsable },
    { "GetPetActionSlotUsable", &Script_GetPetActionSlotUsable },
    { "IsPetAttackActive", &Script_IsPetAttackActive },
    { "PickupPetAction", &Script_PickupPetAction },
    { "TogglePetAutocast", &Script_TogglePetAutocast },
    { "CastPetAction", &Script_CastPetAction },
    { "PetPassiveMode", &Script_PetPassiveMode },
    { "PetDefensiveMode", &Script_PetDefensiveMode },
    { "PetAggressiveMode", &Script_PetAggressiveMode },
    { "PetWait", &Script_PetWait },
    { "PetFollow", &Script_PetFollow },
    { "PetAttack", &Script_PetAttack },
    { "PetStopAttack", &Script_PetStopAttack },
    { "PetAbandon", &Script_PetAbandon },
    { "PetDismiss", &Script_PetDismiss },
    { "PetRename", &Script_PetRename },
    { "PetCanBeAbandoned", &Script_PetCanBeAbandoned },
    { "PetCanBeDismissed", &Script_PetCanBeDismissed },
    { "PetCanBeRenamed", &Script_PetCanBeRenamed },
    { "GetPetTimeRemaining", &Script_GetPetTimeRemaining },
    { "HasPetUI", &Script_HasPetUI },
    { "GetPetExperience", &Script_GetPetExperience },
    { "GetPetHappiness", &Script_GetPetHappiness },
    { "GetPetFoodTypes", &Script_GetPetFoodTypes },
    { "GetPetIcon", &Script_GetPetIcon },
    { "GetPetTalentTree", &Script_GetPetTalentTree },
    { "GetPossessInfo", &Script_GetPossessInfo },
    { "IsPetAttackAction", &Script_IsPetAttackAction },
};

// VERIFY: c9cfe2ae auto
// OFFSET: 0x005D31F0
void PetInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
