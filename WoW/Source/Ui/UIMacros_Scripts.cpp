#include "UIMacros.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"

// VERIFY: 3dacc7a5 auto
// OFFSET: 0x00564B90
static int32_t Script_StopMacro(lua_State* L) {
    if (CGGameUI::CanPerformAction(3)) {
        CGUIMacros::m_macroButton = nullptr;
        CGUIMacros::m_macroRunning = nullptr;
    }

    return 0;
}

// VERIFY: ca803391 auto
// OFFSET: 0x00564C70
static int32_t Script_GetRunningMacroButton(lua_State* L) {
    lua_pushstring(L, CGUIMacros::m_macroRunning);
    return 1;
}

// VERIFY: 42a0c983 manual
// OFFSET: 0x00564BB0
static int32_t Script_GetNumMacros(lua_State* L) {
    lua_pushnumber(L, CGUIMacros::m_numMacros);
    lua_pushnumber(L, CGUIMacros::m_numCharacterMacros);
    return 2;
}

// VERIFY: c78f099b auto
// OFFSET: 0x00564C10
static int32_t Script_GetRunningMacro(lua_State* L) {
    uint32_t id;

    if (CGUIMacros::m_macroButton && (id = CGUIMacros::m_macroButton->m_id)) {
        lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(id) + 1);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 5ba8c11b manual
// OFFSET: 0x00566DF0
static MACRONODE* GetMacroFromLua(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < 72) {
            return CGUIMacros::GetMacroByID(CGUIMacros::m_sortedMacros[index]);
        }
    } else if (lua_isstring(L, 1)) {
        return CGUIMacros::FindMacro(lua_tolstring(L, 1, nullptr));
    }

    return nullptr;
}

// VERIFY: 2e567c78 auto
// OFFSET: 0x00567320
static int32_t Script_SetMacroSpell(lua_State* L) {
    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetMacroSpell(macro, spell [,target])");
        return 0;
    }

    MACRONODE* macro = GetMacroFromLua(L);

    if (!macro) {
        return 0;
    }

    int32_t spellId = GetSpellIdByName(lua_tolstring(L, 2, nullptr), &macro->m_spellFlags);

    if (!spellId) {
        spellId = -1;
    }

    WOWGUID target = { 0 };
    Script_GetGUIDFromToken(lua_tolstring(L, 3, nullptr), target, false);

    if (macro->m_itemID > 0 || spellId != macro->m_spellID || target != macro->m_target) {
        macro->m_target = target;
        macro->m_itemID = 0;
        macro->m_spellID = spellId;
        CGActionBar::UpdateMacro(macro->m_id);
    }

    if (macro->m_dynamic) {
        macro->m_dynamic = 0;
        CGUIMacros::RemoveDynamicMacro(macro);
    }

    if (spellId > 0) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 90bfdb92 auto
// OFFSET: 0x00567450
static int32_t Script_GetMacroSpell(lua_State* L) {
    MACRONODE* macro = GetMacroFromLua(L);

    if (!macro) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(macro->m_spellID, &spell)) {
        lua_pushstring(L, spell.m_name);
        lua_pushstring(L, spell.m_nameSubtext);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

// STATIC: 0x00ACE1F0
static FrameScript_Method s_ScriptFunctions[] = {
    { "SecureCmdOptionParse", &Script_SecureCmdOptionParse },
    { "RunMacro", &Script_RunMacro },
    { "RunMacroText", &Script_RunMacroText },
    { "StopMacro", &Script_StopMacro },
    { "CreateMacro", &Script_CreateMacro },
    { "GetNumMacros", &Script_GetNumMacros },
    { "GetMacroInfo", &Script_GetMacroInfo },
    { "GetMacroBody", &Script_GetMacroBody },
    { "DeleteMacro", &Script_DeleteMacro },
    { "EditMacro", &Script_EditMacro },
    { "SetMacroItem", &Script_SetMacroItem },
    { "GetMacroItem", &Script_GetMacroItem },
    { "SetMacroSpell", &Script_SetMacroSpell },
    { "GetMacroSpell", &Script_GetMacroSpell },
    { "GetNumMacroIcons", &Script_GetNumMacroIcons },
    { "GetNumMacroItemIcons", &Script_GetNumMacroItemIcons },
    { "GetMacroIconInfo", &Script_GetMacroIconInfo },
    { "GetMacroItemIconInfo", &Script_GetMacroItemIconInfo },
    { "PickupMacro", &Script_PickupMacro },
    { "GetMacroIndexByName", &Script_GetMacroIndexByName },
    { "GetRunningMacro", &Script_GetRunningMacro },
    { "GetRunningMacroButton", &Script_GetRunningMacroButton },
};

// VERIFY: 72f0afdb auto
// OFFSET: 0x00564C90
void UIMacrosRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
