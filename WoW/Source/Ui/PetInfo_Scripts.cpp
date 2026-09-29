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

// VERIFY: 57e9bead manual
// OFFSET: 0x005D4EC0
int32_t Script_GetPetActionInfo(lua_State* L) {
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
