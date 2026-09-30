#ifndef WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H
#define WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

int32_t Script_CloseTabardCreation(lua_State* L);
void TabardCreationRegisterScriptFunctions();

class CGTabardCreationFrame {
    public:
    static void EnterWorld();
    static WOWGUID m_vendor;
};

uint32_t GuildGetTabardCost();

#endif
