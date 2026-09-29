#include "CGWorldStateInfo.h"
#include "Engine/Source/Os/TimeManager.h"

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

// VERIFY: 50fb45a8 auto
// OFFSET: 0x00548570
void CGWorldStateInfo::InitializeGame() {
    CGWorldStateInfo::m_mapID = -1;
    CGWorldStateInfo::m_areaID = 0;
    CGWorldStateInfo::m_nextUpdateTime = OsGetAsyncTimeMs();
    CGWorldStateInfo::m_UITimeOffset = 0;
    CGWorldStateInfo::m_nextUITimeUpdate = 0;
}
