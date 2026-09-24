#include <cstdint>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

// STATIC: 0x00B33730
Blizzard::Thread::TLSSlot* System_Thread::s_slotList[128];

// STATIC: 0x00B33948
uint32_t System_Thread::s_slotListUsed;
