#include "CGWorldStateInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
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

// STATIC: 0x00BE8F80
TSGrowableArray<WorldStateUIEntry> CGWorldStateInfo::m_worldStateUI;

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

// VERIFY: 8d223bd3 manual
// OFFSET: 0x00548760
static void SendWorldStateUITimerUpdate() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UI_TIME_REQUEST));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: eeea7d70 auto
// OFFSET: 0x005488F0
void CheckEventWorldStateUITimerUpdate() {
    uint32_t now = OsGetAsyncTimeMs();

    if (static_cast<int32_t>(now - CGWorldStateInfo::m_nextUITimeUpdate) >= 0) {
        CGWorldStateInfo::m_nextUITimeUpdate = now + 20000;
        SendWorldStateUITimerUpdate();
    }

    if (static_cast<int32_t>(now - CGWorldStateInfo::m_nextUpdateTime) < 0) {
        return;
    }

    CGWorldStateInfo::m_nextUpdateTime = now + 1000;

    for (uint32_t i = 0; i < CGWorldStateInfo::m_worldStateUI.m_count; i++) {
        if (CGWorldStateInfo::m_worldStateUI.m_data[i].m_state == 3) {
            FrameScript_SignalEvent(SCRIPT_EVENT_WORLD_STATE_UI_TIMER_UPDATE, nullptr);
            return;
        }
    }
}
