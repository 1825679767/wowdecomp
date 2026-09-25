#if defined(WOWDECOMP_ARCH_64)

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Os/Gui.h"

// STATIC: 0x00D41620
static HWND s_GxDevWindow;

// OFFSET: 0x0086C6A0
void* OsGuiGetWindow(int32_t type) {
    switch (type) {
        case 0:
            return s_GxDevWindow;

        case 1:
            return GetActiveWindow();

        case 2:
            return GetForegroundWindow();

        default:
            return nullptr;
    }
}

// OFFSET: 0x0086C7D0
int32_t OsGuiIsModifierKeyDown(int32_t key) {
    switch (key) {
        case 0:
            return (GetKeyState(VK_CONTROL) & 0xF000) != 0;

        case 1:
            return (GetKeyState(VK_SHIFT) & 0xF000) != 0;

        case 2:
            return (GetKeyState(VK_MENU) & 0xF000) != 0;

        default:
            return 0;
    }
}

#endif
