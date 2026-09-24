#include <windows.h>
#include "Storm/H/storm.h"

// VERIFY: 75275781 auto
// OFFSET: 0x00773A30
HINSTANCE STORMAPI StormGetInstance() {
    return GetModuleHandleA(nullptr);
}
