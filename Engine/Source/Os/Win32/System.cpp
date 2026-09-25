#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/System.h"

// VERIFY: 8e68074b auto
// OFFSET: 0x0086B240
uint32_t OsGetNumberOfProcessors() {
    SYSTEM_INFO info = {};
    GetSystemInfo(&info);

    if (!info.dwNumberOfProcessors) {
        return 1;
    }

    return info.dwNumberOfProcessors;
}

#endif
