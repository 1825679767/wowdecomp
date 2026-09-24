#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// VERIFY: 05456e98 auto
// OFFSET: 0x004283A0
void Blizzard::Lock::MutexCreate(Mutex& mutex) {
    InitializeCriticalSection(&mutex);
}

// VERIFY: b2d016d7 auto
// OFFSET: 0x004283B0
void Blizzard::Lock::MutexEnter(Mutex& mutex) {
    EnterCriticalSection(&mutex);
}

// VERIFY: 82d1a4f7 auto
// OFFSET: 0x004283C0
void Blizzard::Lock::MutexLeave(Mutex& mutex) {
    LeaveCriticalSection(&mutex);
}

// VERIFY: f79b642c auto
// OFFSET: 0x004283D0
void Blizzard::Lock::MutexDestroy(Mutex& mutex) {
    DeleteCriticalSection(&mutex);
    mutex.DebugInfo = nullptr;
}
