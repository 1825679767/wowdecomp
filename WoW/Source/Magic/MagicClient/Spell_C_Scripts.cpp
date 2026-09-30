#include "Spell_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: bf97997d auto
// OFFSET: 0x007FDCD0
static int32_t Script_SpellIsTargeting(lua_State* L) {
    if (s_targetingSpell) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2d5e9a32 auto
// OFFSET: 0x007FDD10
static int32_t Script_SpellCanTargetItem(lua_State* L) {
    if (s_targetingSpell && (s_needTargets & 0x4010)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: caa8802f manual
// OFFSET: 0x007FDD60
static int32_t Script_SpellCanTargetGlyph(lua_State* L) {
    if (s_targetingSpell && (s_needTargets & 0x20000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 537b33f9 manual
// OFFSET: 0x00802270
static int32_t Script_CannotBeResurrected(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_auraEffectMask[314 / 8] & (1 << (314 % 8)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00AF51C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "SpellIsTargeting", &Script_SpellIsTargeting },
    { "SpellCanTargetItem", &Script_SpellCanTargetItem },
    { "SpellTargetItem", &Script_SpellTargetItem },
    { "SpellCanTargetUnit", &Script_SpellCanTargetUnit },
    { "SpellTargetUnit", &Script_SpellTargetUnit },
    { "SpellCanTargetGlyph", &Script_SpellCanTargetGlyph },
    { "SpellStopTargeting", &Script_SpellStopTargeting },
    { "SpellStopCasting", &Script_SpellStopCasting },
    { "CancelUnitBuff", &Script_CancelUnitBuff },
    { "CancelItemTempEnchantment", &Script_CancelItemTempEnchantment },
    { "CannotBeResurrected", &Script_CannotBeResurrected },
};

// VERIFY: c99a9227 auto
// OFFSET: 0x007FDDB0
void SpellRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
