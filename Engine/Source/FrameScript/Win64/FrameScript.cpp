#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D415B8
static uintptr_t s_textStart;

// STATIC: 0x00D415BC
static uintptr_t s_textEnd;

// OFFSET: 0x0086B510
void FrameScript_FindTextSectionBounds() {
    uint8_t* image = reinterpret_cast<uint8_t*>(StormGetInstance());
    IMAGE_NT_HEADERS* header = reinterpret_cast<IMAGE_NT_HEADERS*>(image + reinterpret_cast<IMAGE_DOS_HEADER*>(image)->e_lfanew);
    IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(header);
    uint16_t i;

    for (i = 0; i < header->FileHeader.NumberOfSections; i++) {
        if (!strncmp(reinterpret_cast<const char*>(sections[i].Name), ".text", 8)) {
            break;
        }
    }

    if (i >= header->FileHeader.NumberOfSections) {
        SErrDisplayAppFatal("Unable to find .text section");
    }

    s_textStart = header->OptionalHeader.ImageBase + sections[i].VirtualAddress;
    s_textEnd = header->OptionalHeader.ImageBase + sections[i].VirtualAddress + sections[i].Misc.VirtualSize;
}

// OFFSET: 0x0086B5A0
void FrameScript_InvalidPtrCheck(const void* ptr) {
    if (!s_textStart || !s_textEnd) {
        FrameScript_FindTextSectionBounds();
    }

    if (reinterpret_cast<uintptr_t>(ptr) < s_textStart || reinterpret_cast<uintptr_t>(ptr) >= s_textEnd) {
        char buffer[64];
        SStrPrintf(buffer, sizeof(buffer), "Invalid function pointer: %p", ptr);
        SErrDisplayAppFatal(buffer);
    }
}

#endif
