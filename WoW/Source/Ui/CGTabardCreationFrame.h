#ifndef WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H
#define WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H

#include <cstdint>

struct lua_State;

int32_t Script_CloseTabardCreation(lua_State* L);
int32_t Script_GetTabardCreationCost(lua_State* L);
void TabardCreationRegisterScriptFunctions();

class CGTabardCreationFrame {
    public:
    static void EnterWorld();
};

#endif
