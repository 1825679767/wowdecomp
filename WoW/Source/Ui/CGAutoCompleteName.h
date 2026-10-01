#ifndef WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H
#define WOW_SOURCE_UI_CGAUTOCOMPLETENAME_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct AUTOCOMPLETE_NAMENODE : public TSLinkedNode<AUTOCOMPLETE_NAMENODE> {
    /* 0008 */ char m_name[312];
    /* 0140 */ uint32_t m_lastUsed;
};

class CGAutoCompleteName {
    public:
    static TSList<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>> m_list;
    static bool s_resortNamesOnRecency;
    static void RemoveFlagsByName(const char* name, uint32_t flags, int32_t a3, int32_t a4);
    static bool s_whenEditingFromCenter;
    static bool s_useContext;

    static void ResortAll();
    static void AddFlags(const WOWGUID& guid, uint32_t flags, int32_t add, const char* name);
    static int32_t RemoveFlags(const WOWGUID& guid, uint32_t flags);
};

int32_t Script_GetAutoCompleteResults(lua_State* L);
int32_t Script_GetAutoCompletePresenceID(lua_State* L);
void AutoCompleteRegisterScriptFunctions();

#endif
