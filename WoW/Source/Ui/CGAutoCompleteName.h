#ifndef WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H
#define WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGAutoCompleteName {
    public:
    static bool s_resortNamesOnRecency;
    static void RemoveFlagsByName(const char* name, uint32_t flags, int32_t a3, int32_t a4);
    static bool s_whenEditingFromCenter;
    static bool s_useContext;

    static void ResortAll();
    static void AddFlags(const WOWGUID& guid, uint32_t flags, int32_t add, const char* name);
};

int32_t Script_GetAutoCompleteResults(lua_State* L);
int32_t Script_GetAutoCompletePresenceID(lua_State* L);
void AutoCompleteRegisterScriptFunctions();

#endif
