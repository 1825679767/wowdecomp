#ifndef WOW_SOURCE_UI_CURRENCYTYPES_H
#define WOW_SOURCE_UI_CURRENCYTYPES_H

#include <cstdint>

struct lua_State;

class CGCurrencyTypes {
    public:
    static void InitializeGame();
    static void EnterWorld();
};

int32_t Script_GetCurrencyListSize(lua_State* L);
int32_t Script_GetCurrencyListInfo(lua_State* L);
int32_t Script_ExpandCurrencyList(lua_State* L);
int32_t Script_SetCurrencyUnused(lua_State* L);
int32_t Script_SetCurrencyBackpack(lua_State* L);
int32_t Script_GetBackpackCurrencyInfo(lua_State* L);
void CurrencyTypesRegisterScriptFunctions();

#endif
