#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"

// VERIFY: 3018692c auto
// OFFSET: 0x00428320
void Blizzard::Lock::EventDestroy(Event& event) {
    CloseHandle(event);
}

// VERIFY: 101c672a auto
// OFFSET: 0x00428340
void Blizzard::Lock::EventSet(Event& event) {
    SetEvent(event);
}

// VERIFY: 3c0f7645 auto
// OFFSET: 0x00428360
void Blizzard::Lock::EventWaitForever(Event& event) {
    WaitForSingleObject(event, INFINITE);
}

// VERIFY: 48d5dec5 auto
// OFFSET: 0x00428380
Blizzard::Lock::Event* Blizzard::Lock::EventWait(uint32_t timeout, Event& event) {
    return WaitForSingleObject(event, timeout) == WAIT_OBJECT_0 ? &event : nullptr;
}
