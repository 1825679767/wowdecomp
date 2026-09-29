#ifndef WOW_SOURCE_UI_ARENATEAMINFO_H
#define WOW_SOURCE_UI_ARENATEAMINFO_H

#include <cstdint>

struct lua_State;

class CGArenaTeamInfo {
    public:
    static void EnterWorld();
    static void InitializeGame();
};

int32_t Script_GetArenaTeam(lua_State* L);
int32_t Script_GetNumArenaTeamMembers(lua_State* L);
int32_t Script_GetArenaTeamRosterInfo(lua_State* L);
int32_t Script_GetArenaTeamGdfInfo(lua_State* L);
int32_t Script_SetArenaTeamRosterSelection(lua_State* L);
int32_t Script_GetArenaTeamRosterSelection(lua_State* L);
int32_t Script_SortArenaTeamRoster(lua_State* L);
int32_t Script_SetArenaTeamRosterShowOffline(lua_State* L);
int32_t Script_GetArenaTeamRosterShowOffline(lua_State* L);
int32_t Script_CloseArenaTeamRoster(lua_State* L);
int32_t Script_ArenaTeamRoster(lua_State* L);
int32_t Script_GetCurrentArenaSeason(lua_State* L);
int32_t Script_GetPreviousArenaSeason(lua_State* L);
void ArenaTeamInfoRegisterScriptFunctions();

#endif
