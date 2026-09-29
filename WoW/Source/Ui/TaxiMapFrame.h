#ifndef WOW_SOURCE_UI_TAXIMAPFRAME_H
#define WOW_SOURCE_UI_TAXIMAPFRAME_H

#include <cstddef>
#include <cstdint>

struct lua_State;

class CGTaxiMap {
    public:
    static void InitializeGame();
    static void RegisterScriptFunctions();
};

int32_t Script_SetTaxiMap(lua_State* L);
int32_t Script_NumTaxiNodes(lua_State* L);
int32_t Script_TaxiNodeName(lua_State* L);
int32_t Script_TaxiNodePosition(lua_State* L);
int32_t Script_TaxiNodeCost(lua_State* L);
int32_t Script_TakeTaxiNode(lua_State* L);
int32_t Script_CloseTaxiMap(lua_State* L);
int32_t Script_TaxiNodeGetType(lua_State* L);
int32_t Script_TaxiNodeSetCurrent(lua_State* L);
int32_t Script_TaxiGetSrcX(lua_State* L);
int32_t Script_TaxiGetSrcY(lua_State* L);
int32_t Script_TaxiGetDestX(lua_State* L);
int32_t Script_TaxiGetDestY(lua_State* L);
int32_t Script_GetNumRoutes(lua_State* L);

#endif
