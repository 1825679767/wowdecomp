#include "CharacterCreation.h"
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"

int32_t Script_SetCharCustomizeBackground(lua_State* L);
int32_t Script_ResetCharCustomize(lua_State* L);
int32_t Script_GetNameForRace(lua_State* L);
int32_t Script_GetFactionForRace(lua_State* L);
int32_t Script_GetAvailableRaces(lua_State* L);
int32_t Script_GetAvailableClasses(lua_State* L);
int32_t Script_GetClassesForRace(lua_State* L);
int32_t Script_GetHairCustomization(lua_State* L);
int32_t Script_GetFacialHairCustomization(lua_State* L);
int32_t Script_GetSelectedRace(lua_State* L);
int32_t Script_GetSelectedSex(lua_State* L);
int32_t Script_GetSelectedClass(lua_State* L);
int32_t Script_SetSelectedRace(lua_State* L);
int32_t Script_SetSelectedSex(lua_State* L);
int32_t Script_SetSelectedClass(lua_State* L);
int32_t Script_UpdateCustomizationBackground(lua_State* L);
int32_t Script_UpdateCustomizationScene(lua_State* L);
int32_t Script_CycleCharCustomization(lua_State* L);
int32_t Script_RandomizeCharCustomization(lua_State* L);
int32_t Script_GetCharacterCreateFacing(lua_State* L);
int32_t Script_SetCharacterCreateFacing(lua_State* L);
int32_t Script_GetRandomName(lua_State* L);
int32_t Script_CreateCharacter(lua_State* L);
int32_t Script_CustomizeExistingCharacter(lua_State* L);
int32_t Script_PaidChange_GetPreviousRaceIndex(lua_State* L);
int32_t Script_PaidChange_GetCurrentRaceIndex(lua_State* L);
int32_t Script_PaidChange_GetCurrentClassIndex(lua_State* L);
int32_t Script_PaidChange_GetName(lua_State* L);
int32_t Script_IsRaceClassValid(lua_State* L);
int32_t Script_IsRaceClassRestricted(lua_State* L);
int32_t Script_GetCreateBackgroundModel(lua_State* L);

// VERIFY: a2e71192 auto
// OFFSET: 0x004E05B0
static int32_t Script_SetCharCustomizeFrame(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharCustomizeFrame(\"frameName\")");
        return 0;
    }

    CSimpleModelFFX* frame = static_cast<CSimpleModelFFX*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, nullptr), CSimpleModel::GetObjectType()));

    if (frame) {
        CCharacterCreation::m_charCustomizeFrame = frame;
    }

    return 0;
}

// STATIC: 0x00AC4228
static FrameScript_Method s_scriptFunctions[] = {
    { "SetCharCustomizeFrame", &Script_SetCharCustomizeFrame },
    { "SetCharCustomizeBackground", &Script_SetCharCustomizeBackground },
    { "ResetCharCustomize", &Script_ResetCharCustomize },
    { "GetNameForRace", &Script_GetNameForRace },
    { "GetFactionForRace", &Script_GetFactionForRace },
    { "GetAvailableRaces", &Script_GetAvailableRaces },
    { "GetAvailableClasses", &Script_GetAvailableClasses },
    { "GetClassesForRace", &Script_GetClassesForRace },
    { "GetHairCustomization", &Script_GetHairCustomization },
    { "GetFacialHairCustomization", &Script_GetFacialHairCustomization },
    { "GetSelectedRace", &Script_GetSelectedRace },
    { "GetSelectedSex", &Script_GetSelectedSex },
    { "GetSelectedClass", &Script_GetSelectedClass },
    { "SetSelectedRace", &Script_SetSelectedRace },
    { "SetSelectedSex", &Script_SetSelectedSex },
    { "SetSelectedClass", &Script_SetSelectedClass },
    { "UpdateCustomizationBackground", &Script_UpdateCustomizationBackground },
    { "UpdateCustomizationScene", &Script_UpdateCustomizationScene },
    { "CycleCharCustomization", &Script_CycleCharCustomization },
    { "RandomizeCharCustomization", &Script_RandomizeCharCustomization },
    { "GetCharacterCreateFacing", &Script_GetCharacterCreateFacing },
    { "SetCharacterCreateFacing", &Script_SetCharacterCreateFacing },
    { "GetRandomName", &Script_GetRandomName },
    { "CreateCharacter", &Script_CreateCharacter },
    { "CustomizeExistingCharacter", &Script_CustomizeExistingCharacter },
    { "PaidChange_GetPreviousRaceIndex", &Script_PaidChange_GetPreviousRaceIndex },
    { "PaidChange_GetCurrentRaceIndex", &Script_PaidChange_GetCurrentRaceIndex },
    { "PaidChange_GetCurrentClassIndex", &Script_PaidChange_GetCurrentClassIndex },
    { "PaidChange_GetName", &Script_PaidChange_GetName },
    { "IsRaceClassValid", &Script_IsRaceClassValid },
    { "IsRaceClassRestricted", &Script_IsRaceClassRestricted },
    { "GetCreateBackgroundModel", &Script_GetCreateBackgroundModel },
};

// VERIFY: 88b9bf9d auto
// OFFSET: 0x004DFD00
void CharacterCreateRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}
