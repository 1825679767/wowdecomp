#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <malloc.h>
#include <windows.h>
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D41620
static HWND s_GxDevWindow;

// VERIFY: d132b7cd auto
// OFFSET: 0x0086C650
void OsGuiSetWindowTitle(void* window, const char* title) {
    uint32_t length = SUniConvertUTF8to16Len(title, 0x7FFFFFFF, nullptr);
    uint16_t* wideTitle = static_cast<uint16_t*>(_alloca(length * sizeof(uint16_t)));
    SUniConvertUTF8to16(wideTitle, length, title, 0x7FFFFFFF, nullptr, nullptr);

    SetWindowTextW(static_cast<HWND>(window), reinterpret_cast<const wchar_t*>(wideTitle));
}

// VERIFY: 7fcab45a auto
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

// VERIFY: 3cdad9fe auto
// OFFSET: 0x0086C6D0
void OsGuiSetGxWindow(void* window) {
    s_GxDevWindow = static_cast<HWND>(window);
}

// VERIFY: 39d42e31 auto
// OFFSET: 0x0086C6E0
int32_t OsGuiMessageBox(void* parentWindow, int32_t style, const char* message, const char* title) {
    uint32_t messageLength = SUniConvertUTF8to16Len(message, 0x7FFFFFFF, nullptr);
    uint16_t* wideMessage = static_cast<uint16_t*>(_alloca(messageLength * sizeof(uint16_t)));
    SUniConvertUTF8to16(wideMessage, messageLength, message, 0x7FFFFFFF, nullptr, nullptr);

    if (!title) {
        title = "";
    }

    uint32_t titleLength = SUniConvertUTF8to16Len(title, 0x7FFFFFFF, nullptr);
    uint16_t* wideTitle = static_cast<uint16_t*>(_alloca(titleLength * sizeof(uint16_t)));
    SUniConvertUTF8to16(wideTitle, titleLength, title, 0x7FFFFFFF, nullptr, nullptr);

    UINT type = MB_OK;

    switch (style) {
        case 0:
            type = MB_OK;
            break;

        case 1:
            type = MB_OKCANCEL;
            break;

        case 2:
            type = MB_YESNO;
            break;

        case 3:
            type = MB_YESNOCANCEL;
            break;

        default:
            break;
    }

    switch (MessageBoxW(static_cast<HWND>(parentWindow), reinterpret_cast<const wchar_t*>(wideMessage), reinterpret_cast<const wchar_t*>(wideTitle), type)) {
        case IDOK:
        case IDYES:
            return 0;

        case IDNO:
            return 1;

        default:
            return 2;
    }
}

// VERIFY: 93ff2d97 manual
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
