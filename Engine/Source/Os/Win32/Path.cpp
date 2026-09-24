#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <malloc.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: a5cb9704 manual
// OFFSET: 0x0086BBD0
uint32_t GetModuleFileNameUTF8(void* module, char* path, uint32_t size) {
    uint32_t wideSize = size * 3 * sizeof(uint16_t);
    uint16_t* wide = static_cast<uint16_t*>(_alloca(wideSize));
    wide[0] = 0;
    GetModuleFileNameW(static_cast<HMODULE>(module), reinterpret_cast<LPWSTR>(wide), wideSize);

    uint32_t written;
    SUniConvertUTF16to8(path, size, wide, 0x7FFFFFFF, &written, nullptr);

    if (size - 1 >= written) {
        path[written] = '\0';
    } else {
        path[size - 1] = '\0';
    }

    return written;
}

// VERIFY: 2158e20e manual
// OFFSET: 0x00461DB0
bool OsSetCurrentDirectory(const char* path) {
    if (path) {
        return Blizzard::File::SetWorkingDirectory(path) != 0;
    }

    SetLastError(ERROR_INVALID_PARAMETER);

    return false;
}

#endif
