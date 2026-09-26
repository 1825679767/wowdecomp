#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// VERIFY: 0c90ad19 manual
// OFFSET: 0x00450B80
bool Blizzard::Thread::CreateDetached(uint32_t (*function)(void*), void* param, const char* name) {
    ThreadRecord* thread;
    bool result = Create(function, param, &thread, name, 0, 0);

    if (result) {
        Close(thread);
    }

    return result;
}
