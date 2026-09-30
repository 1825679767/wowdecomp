#include "CGWorldStateInfo.h"
#include "Engine/Source/Os/TimeManager.h"
#include <ctime>

// STATIC: 0x00BE8F40
uint32_t CGWorldStateInfo::m_nextUITimeUpdate;

// STATIC: 0x00BE8F44
uint32_t CGWorldStateInfo::m_UITimeOffset;

// STATIC: 0x00BE8F48
uint32_t CGWorldStateInfo::m_nextUpdateTime;

// STATIC: 0x00BE8F50
int32_t CGWorldStateInfo::m_areaID;

// STATIC: 0x00BE8F54
int32_t CGWorldStateInfo::m_mapID;

// STATIC: 0x00BE8F58
TSHashTable<WORLDSTATE, HASHKEY_NONE> CGWorldStateInfo::m_worldStates;

// VERIFY: 50fb45a8 auto
// OFFSET: 0x00548570
void CGWorldStateInfo::InitializeGame() {
    CGWorldStateInfo::m_mapID = -1;
    CGWorldStateInfo::m_areaID = 0;
    CGWorldStateInfo::m_nextUpdateTime = OsGetAsyncTimeMs();
    CGWorldStateInfo::m_UITimeOffset = 0;
    CGWorldStateInfo::m_nextUITimeUpdate = 0;
}

// VERIFY: a62d0161 auto
// OFFSET: 0x00548700
int32_t CGWorldStateInfo::GetUITime() {
    return CGWorldStateInfo::m_UITimeOffset + static_cast<int32_t>(time(nullptr));
}

// VERIFY: dd8a89da auto
// OFFSET: 0x00548D10
int32_t GetWorldState(int32_t id) {
    WORLDSTATE* state = CGWorldStateInfo::m_worldStates.Ptr(id, HASHKEY_NONE());

    if (state) {
        return state->m_value;
    }

    return 0;
}
