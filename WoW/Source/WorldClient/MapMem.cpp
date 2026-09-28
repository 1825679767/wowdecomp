#include "MapMem.h"
#include "WoW/Source/WorldClient/MapArea.h"

// VERIFY: bffb685d manual
// OFFSET: 0x007BFE40
void* CMapArea::AllocAsyncLoadBuffer(int32_t fileSize) {
    return SMemAlloc(fileSize, __FILE__, __LINE__, 0);
}
