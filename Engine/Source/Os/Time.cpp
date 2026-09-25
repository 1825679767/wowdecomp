#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"

// VERIFY: b4efc621 auto
// OFFSET: 0x0086D430
void OsTimeStartup(OsTimeManager::TimingMethod method) {
    OsTimeManagerCreate(method);
}

// VERIFY: f2a82140 auto
// OFFSET: 0x0086D440
void OsTimeShutdown() {
    OsTimeManagerDestroy();
}
