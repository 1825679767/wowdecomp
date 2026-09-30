#include "SpellBookFrame.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"

// STATIC: 0x00ACCCC4
static const char* s_companionTypes[2] = { "CRITTER", "MOUNT" };

// VERIFY: d9335da2 manual
// OFFSET: 0x0053C0B0
static int32_t Script_GetNumShapeshiftForms(lua_State* L) {
    lua_pushnumber(L, s_shapeshiftSpells.m_count);
    return 1;
}

// VERIFY: a27f3d8b auto
// OFFSET: 0x0053DD10
static int32_t Script_UpdateSpells(lua_State* L) {
    CGSpellBook::UpdateSpells(0, 0, 0);
    return 0;
}

// VERIFY: dd465bdc manual
// OFFSET: 0x0053B5C0
static int32_t Script_GetNumSpellTabs(lua_State* L) {
    lua_pushnumber(L, s_numSkillInfos);
    return 1;
}

// VERIFY: 0a46e79d manual
// OFFSET: 0x0053B6C0
static int32_t Script_FindSpellBookSlotByID(lua_State* L) {
    int32_t spellId;

    if (!lua_isnumber(L, 1) || (spellId = static_cast<int32_t>(lua_tointeger(L, 1))) <= 0) {
        luaL_error(L, "Usage: FindSpellBookSlotByID(spellID[, isPet])");
        return 0;
    }

    int32_t slot = GetSpellSlot(spellId, StringToBOOL(L, 2, 0));

    if (slot >= 0) {
        lua_pushinteger(L, slot);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 6fb2c8d5 auto
// OFFSET: 0x0053B650
static int32_t Script_GetKnownSlotFromHighestRankSlot(lua_State* L) {
    uint32_t slot = lua_tointeger(L, 1) - 1;
    int32_t index = -1;

    if (slot < s_unkBE8D9C) {
        int32_t spell = s_unkBE5D88[slot];

        for (uint32_t i = 0; i < s_numSpells; i++) {
            if (s_spellList[i] == spell) {
                index = i;
                break;
            }
        }
    }

    lua_pushnumber(L, index + 1);
    return 1;
}

// VERIFY: 809ea20d manual
// OFFSET: 0x0053BE70
static int32_t Script_GetSpellTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSpellTabInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    SpellBookSkillInfo* info;

    if (static_cast<uint32_t>(index) >= s_numSkillInfos || !(info = s_skillInfos.m_data[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 6;
    }

    if (!info->m_skillLine) {
        lua_pushstring(L, FrameScript_GetText("GENERAL_SPELLS", -1, GENDER_NOT_APPLICABLE));
        lua_pushstring(L, "Interface\\Icons\\Ability_Kick");
    } else {
        SkillLineRec* skill = g_skillLineDB.GetRecord(info->m_skillLine);

        if (skill) {
            lua_pushstring(L, skill->m_displayName);
            SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushnil(L);
            }
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    uint32_t offset = 0;

    if (index && static_cast<uint32_t>(index) < s_numSkillInfos) {
        for (int32_t i = index - 1; i >= 0; i--) {
            offset += s_skillInfos.m_data[i]->m_count;
        }
    }

    lua_pushnumber(L, offset);
    lua_pushnumber(L, info->m_count);
    uint32_t visibleOffset = 0;

    if (index && static_cast<uint32_t>(index) < s_numSkillInfos) {
        for (int32_t i = index - 1; i >= 0; i--) {
            visibleOffset += s_skillInfos.m_data[i]->m_unk08;
        }
    }

    lua_pushnumber(L, visibleOffset);
    lua_pushnumber(L, info->m_unk08);
    return 6;
}

// VERIFY: 18466665 auto
// OFFSET: 0x0053B600
static bool GetLuaCompanionType(lua_State* L, int32_t* type) {
    const char* token = lua_tolstring(L, 1, nullptr);

    if (!token) {
        return false;
    }

    for (int32_t i = 0; i < 2; i++) {
        if (!SStrCmpI(token, s_companionTypes[i], STORM_MAX_STR)) {
            *type = i;
            return true;
        }
    }

    return false;
}

// VERIFY: 5b1a870f manual
// OFFSET: 0x0053DD30
static int32_t Script_HasPetSpells(lua_State* L) {
    if (!CGSpellBook::m_numPetSpells) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnumber(L, CGSpellBook::m_numPetSpells);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushstring(L, "PET");
        return 2;
    }

    ChrClassesRec* rec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);
    lua_pushstring(L, rec->m_petNameToken);
    return 2;
}

// VERIFY: f846068d manual
// OFFSET: 0x0053E2C0
static int32_t Script_GetCompanionInfo(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: GetCompanionInfo(type, index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];

    if (index >= companions.m_count) {
        return 0;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(companions.m_data[index], &spell)) {
        return 0;
    }

    int32_t creatureId = spell.m_effectMiscValue[0];
    lua_pushinteger(L, creatureId);
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(creatureId, WOWGUID(), nullptr, nullptr, false);

    if (stats) {
        lua_pushstring(L, stats->GetName(0));
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, spell.m_id);
    bool active = Spell_C_IsActiveSpell(&spell, ClntObjMgrGetActivePlayerObj());
    SpellIconRec* icon = g_spellIconDB.GetRecord(active && spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    if (active) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// STATIC: 0x00ACCCE0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumSpellTabs", &Script_GetNumSpellTabs },
    { "GetSpellTabInfo", &Script_GetSpellTabInfo },
    { "GetSpellName", &Script_GetSpellName },
    { "GetSpellLink", &Script_GetSpellLink },
    { "GetSpellInfo", &Script_GetSpellInfo },
    { "GetSpellTexture", &Script_GetSpellTexture },
    { "GetSpellCount", &Script_GetSpellCount },
    { "GetSpellCooldown", &Script_GetSpellCooldown },
    { "GetSpellAutocast", &Script_GetSpellAutocast },
    { "ToggleSpellAutocast", &Script_ToggleSpellAutocast },
    { "EnableSpellAutocast", &Script_EnableSpellAutocast },
    { "DisableSpellAutocast", &Script_DisableSpellAutocast },
    { "PickupSpell", &Script_PickupSpell },
    { "CastSpell", &Script_CastSpell },
    { "IsSelectedSpell", &Script_IsSelectedSpell },
    { "IsPassiveSpell", &Script_IsPassiveSpell },
    { "IsAttackSpell", &Script_IsAttackSpell },
    { "IsCurrentSpell", &Script_IsCurrentSpell },
    { "IsAutoRepeatSpell", &Script_IsAutoRepeatSpell },
    { "IsUsableSpell", &Script_IsUsableSpell },
    { "IsHelpfulSpell", &Script_IsHelpfulSpell },
    { "IsHarmfulSpell", &Script_IsHarmfulSpell },
    { "IsConsumableSpell", &Script_IsConsumableSpell },
    { "SpellHasRange", &Script_SpellHasRange },
    { "IsSpellInRange", &Script_IsSpellInRange },
    { "UpdateSpells", &Script_UpdateSpells },
    { "HasPetSpells", &Script_HasPetSpells },
    { "GetNumShapeshiftForms", &Script_GetNumShapeshiftForms },
    { "GetShapeshiftForm", &Script_GetShapeshiftForm },
    { "CancelShapeshiftForm", &Script_CancelShapeshiftForm },
    { "GetShapeshiftFormInfo", &Script_GetShapeshiftFormInfo },
    { "CastShapeshiftForm", &Script_CastShapeshiftForm },
    { "GetShapeshiftFormCooldown", &Script_GetShapeshiftFormCooldown },
    { "CastSpellByName", &Script_CastSpellByName },
    { "CastSpellByID", &Script_CastSpellByID },
    { "GetNumCompanions", &Script_GetNumCompanions },
    { "GetCompanionInfo", &Script_GetCompanionInfo },
    { "GetCompanionCooldown", &Script_GetCompanionCooldown },
    { "PickupCompanion", &Script_PickupCompanion },
    { "CallCompanion", &Script_CallCompanion },
    { "DismissCompanion", &Script_DismissCompanion },
    { "GetKnownSlotFromHighestRankSlot", &Script_GetKnownSlotFromHighestRankSlot },
    { "IsSpellKnown", &Script_IsSpellKnown },
    { "FindSpellBookSlotByID", &Script_FindSpellBookSlotByID },
    { "SummonRandomCritter", &Script_SummonRandomCritter },
};

// VERIFY: e97b7dd8 auto
// OFFSET: 0x0053B740
void SpellBookRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
