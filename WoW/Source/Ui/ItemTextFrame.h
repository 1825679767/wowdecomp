#ifndef WOW_SOURCE_UI_ITEMTEXTFRAME_H
#define WOW_SOURCE_UI_ITEMTEXTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"

struct lua_State;

class CGItemText {
    public:
    static void InitializeGame();
    static WOWGUID m_itemGUID;
    static uint32_t m_currentPage;
    static uint8_t m_hasItemText;
    static char m_text[8000];
    static TSGrowableArray<int32_t> m_pages;

    static void SetItem(const WOWGUID& guid, int32_t a2);
};

int32_t Script_ItemTextGetCreator(lua_State* L);
int32_t Script_ItemTextGetMaterial(lua_State* L);
int32_t Script_ItemTextPrevPage(lua_State* L);
int32_t Script_ItemTextNextPage(lua_State* L);
int32_t Script_CloseItemText(lua_State* L);
void ItemTextRegisterScriptFunctions();

#endif
