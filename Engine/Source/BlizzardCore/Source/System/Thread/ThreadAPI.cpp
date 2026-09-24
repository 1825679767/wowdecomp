#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"

// VERIFY: aa42bb09 auto
// OFFSET: 0x0044B770
bool Blizzard::Thread::Close(ThreadRecord* thread) {
    if (thread->m_refCount.Decrement() > 0) {
        return true;
    }

    if (thread->m_handle) {
        CloseHandle(thread->m_handle);
        thread->m_handle = nullptr;
    }

    System_Thread::RemoveFromRegistry(thread);
    free(thread);

    return true;
}

// VERIFY: 0c90ad19 auto
// OFFSET: 0x00450B80
bool Blizzard::Thread::CreateDetached(uint32_t (*function)(void*), void* param, const char* name) {
    ThreadRecord* thread;
    bool result = Create(function, param, &thread, name, 0, 0);

    if (result) {
        Close(thread);
    }

    return result;
}
