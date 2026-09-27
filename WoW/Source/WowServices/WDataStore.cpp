#include "WDataStore.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B38CF0
static uint32_t* s_smallBufferHeap;

// STATIC: 0x00B38CF4
static uint32_t* s_largeBufferHeap;

// STATIC: 0x00B38CF8
static uint32_t* s_superBufferHeap;

// STATIC: 0x00B38CFC
static bool s_initialized;

// VERIFY: 4d46f8e8 manual
// OFFSET: 0x00466050
void WDataStore::StaticInitialize() {
    if (s_initialized) {
        return;
    }

    s_smallBufferHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(800, 256, "WDataStoreSmallBuffer", true));
    s_largeBufferHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(0x4020, 16, "WDataStoreLargeBuffer", true));
    s_superBufferHeap = new (__FILE__, __LINE__) uint32_t(ObjectAllocAddHeap(0x40060, 4, "WDataStoreSuperBuffer", true));
    s_initialized = true;
}
