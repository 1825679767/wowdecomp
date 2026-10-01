#ifndef WOW_SOURCE_UI_CGWORLDSTATEINFO_H
#define WOW_SOURCE_UI_CGWORLDSTATEINFO_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct lua_State;

struct WORLDSTATE : public TSHashObject<WORLDSTATE, HASHKEY_NONE> {
    /* 0018 */ int32_t m_value;
};

struct WorldStateUIEntry {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_state;
};

int32_t GetWorldState(int32_t id);

class CGWorldStateInfo {
    public:
    static int32_t m_mapID;
    static int32_t m_areaID;
    static uint32_t m_nextUpdateTime;
    static uint32_t m_UITimeOffset;
    static uint32_t m_nextUITimeUpdate;
    static TSHashTable<WORLDSTATE, HASHKEY_NONE> m_worldStates;
    static TSGrowableArray<WorldStateUIEntry> m_worldStateUI;

    static int32_t GetUITime();
    static void InitializeGame();
};

int32_t Script_GetWorldStateUIInfo(lua_State* L);
void WorldStateInfoRegisterScriptFunctions();

void CheckEventWorldStateUITimerUpdate();

#endif
