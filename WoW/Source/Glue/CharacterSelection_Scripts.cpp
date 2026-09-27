#include "CharacterSelection.h"
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"

int32_t Script_SetCharSelectBackground(lua_State* L);
int32_t Script_GetCharacterListUpdate(lua_State* L);
int32_t Script_GetCharacterInfo(lua_State* L);
int32_t Script_SelectCharacter(lua_State* L);
int32_t Script_DeleteCharacter(lua_State* L);
int32_t Script_RenameCharacter(lua_State* L);
int32_t Script_DeclineCharacter(lua_State* L);
int32_t Script_UpdateSelectionCustomizationScene(lua_State* L);
int32_t Script_GetCharacterSelectFacing(lua_State* L);
int32_t Script_SetCharacterSelectFacing(lua_State* L);
int32_t Script_GetSelectBackgroundModel(lua_State* L);

// VERIFY: 769d9898 auto
// OFFSET: 0x004E2F60
static int32_t Script_SetCharSelectModelFrame(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharSelectModelFrame(\"frameName\")");
        return 0;
    }

    CSimpleModelFFX* frame = static_cast<CSimpleModelFFX*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, nullptr), CSimpleModel::GetObjectType()));

    if (frame) {
        CCharacterSelection::m_modelFrame = frame;
    }

    return 0;
}

// VERIFY: 68a2a930 manual
// OFFSET: 0x004E3140
static int32_t Script_GetNumCharacters(lua_State* L) {
    lua_pushnumber(L, CCharacterSelection::s_characterList.Count());
    return 1;
}

// STATIC: 0x00AC4378
static FrameScript_Method s_scriptFunctions[] = {
    { "SetCharSelectModelFrame", &Script_SetCharSelectModelFrame },
    { "SetCharSelectBackground", &Script_SetCharSelectBackground },
    { "GetCharacterListUpdate", &Script_GetCharacterListUpdate },
    { "GetNumCharacters", &Script_GetNumCharacters },
    { "GetCharacterInfo", &Script_GetCharacterInfo },
    { "SelectCharacter", &Script_SelectCharacter },
    { "DeleteCharacter", &Script_DeleteCharacter },
    { "RenameCharacter", &Script_RenameCharacter },
    { "DeclineCharacter", &Script_DeclineCharacter },
    { "UpdateSelectionCustomizationScene", &Script_UpdateSelectionCustomizationScene },
    { "GetCharacterSelectFacing", &Script_GetCharacterSelectFacing },
    { "SetCharacterSelectFacing", &Script_SetCharacterSelectFacing },
    { "GetSelectBackgroundModel", &Script_GetSelectBackgroundModel },
};

// VERIFY: 55f0de4b auto
// OFFSET: 0x004E2570
void CharSelectRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}
