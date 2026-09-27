#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"

// OFFSET: 0x004294C0
char* GetComponentVersionData(uint32_t* size) {
    *size = 0;

    HRSRC resource = FindResourceExA(nullptr, "BLIZZARDCOMPONENT", "BLIZZARDCOMPONENT", 0);

    if (!resource) {
        return nullptr;
    }

    HGLOBAL handle = LoadResource(nullptr, resource);

    if (!handle) {
        return nullptr;
    }

    const void* source = LockResource(handle);
    *size = SizeofResource(nullptr, resource);

    char* data = new char[*size + 1];
    data[*size] = '\0';
    memcpy(data, source, *size);

    return data;
}

#endif
