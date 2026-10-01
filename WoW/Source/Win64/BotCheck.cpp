#include <intrin.h>
#include <cstdint>
#include "Engine/Source/Os/OsLibrary.h"
#include "Engine/Source/Os/System.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Net/Types.h"

// OFFSET: 0x004046C0
void AdditionalCheck1() {
    if (!s_botCheckEnabled || s_botCheckDelay <= 0 || s_botCheckDelay > 420) {
        return;
    }

    if (_InterlockedCompareExchange(&s_additionalCheckArmed, 0, 1) != 1) {
        return;
    }

    AdditionalCheckInterface iface;
    iface.m_memAlloc = Client_MemAlloc;
    iface.m_memFree = Client_MemFree;
    iface.m_loadLibrary = OsLoadLibrary;
    iface.m_getLibraryEntryPoint = OsGetLibraryEntryPoint;
    iface.m_freeLibrary = OsFreeLibrary;
    uint8_t detected = 0;

    __try {
        if (*reinterpret_cast<const uint8_t*>(&OsValidateFunctionPointer) == 0xE9) {
            return;
        }

        detected = AdditionalCheck1Scan(&iface, 0);
    } __except (1) {
    }

    if (detected) {
        SendBotDetected2(CMSG_BOT_DETECTED2, 0x60231D69);
    }
}
