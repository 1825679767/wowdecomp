#include "ItemTextFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

// VERIFY: d3517090 auto
// OFFSET: 0x00589B50
static int32_t Script_ItemTextGetPage(lua_State* L) {
    lua_pushnumber(L, CGItemText::m_currentPage + 1);
    return 1;
}

// VERIFY: ad27c4fc manual
// OFFSET: 0x00589C40
static int32_t Script_ItemTextGetItem(lua_State* L) {
    const char* name = nullptr;
    CGObject_C* object = ClntObjMgrObjectPtr(CGItemText::m_itemGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        name = object->GetObjectName();
    }

    lua_pushstring(L, name);
    return 1;
}

// VERIFY: 4f9393fe auto
// OFFSET: 0x00589B90
static int32_t Script_ItemTextGetText(lua_State* L) {
    lua_pushstring(L, CGItemText::m_text);
    return 1;
}

// VERIFY: 8b69700f auto
// OFFSET: 0x00589D70
static int32_t Script_ItemTextHasNextPage(lua_State* L) {
    if (CGItemText::m_hasItemText || !CGItemText::m_pages.m_data[CGItemText::m_currentPage + 1]) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// STATIC: 0x00ACEEC0
static FrameScript_Method s_ScriptFunctions[] = {
    { "ItemTextGetItem", &Script_ItemTextGetItem },
    { "ItemTextGetCreator", &Script_ItemTextGetCreator },
    { "ItemTextGetMaterial", &Script_ItemTextGetMaterial },
    { "ItemTextGetPage", &Script_ItemTextGetPage },
    { "ItemTextGetText", &Script_ItemTextGetText },
    { "ItemTextHasNextPage", &Script_ItemTextHasNextPage },
    { "ItemTextPrevPage", &Script_ItemTextPrevPage },
    { "ItemTextNextPage", &Script_ItemTextNextPage },
    { "CloseItemText", &Script_CloseItemText },
};

// VERIFY: 65411387 auto
// OFFSET: 0x00589BB0
void ItemTextRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
