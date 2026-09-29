#include "SpellBookFrame.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

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

// VERIFY: fee8ae09 manual
// OFFSET: 0x0053C0B0
int32_t Script_GetNumShapeshiftForms(lua_State* L) {
    lua_pushnumber(L, s_shapeshiftSpells.m_count);
    return 1;
}
