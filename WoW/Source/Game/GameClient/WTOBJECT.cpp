#include "WTOBJECT.h"
#include "Engine/Source/Base/CDataAllocator.h"

// STATIC: 0x00D38078
static CDataAllocator s_freeSwings(sizeof(SWING), 8);

// STATIC: 0x00D3808C
static CDataAllocator s_freeTrails(sizeof(WTOBJECT), 128);

// VERIFY: c3c7b136 auto
// OFFSET: 0x007E4BC0
void WeaponTrailsShutdown() {
    s_freeTrails.Clear(typeid(WTOBJECT).raw_name(), SLOG_OBJECT, 1);
    s_freeSwings.Clear(typeid(SWING).raw_name(), SLOG_OBJECT, 1);
}
