#if defined(WOWDECOMP_ARCH_32)

#include "WoW/Source/Game/GameClient/WardenClient.h"
#include "Engine/Source/Os/BLLLoader.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00AF29AC
static WardenClient::OurLibrary s_ourLibrary;

// STATIC: 0x00D31A44
static BLLLoader* s_pendingModule;

// STATIC: 0x00D31A48
static BLLLoader* s_activeModule;

// STATIC: 0x00D31A4C
static WardenModule* s_moduleInterface;

// STATIC: 0x00D31A58
static uint32_t s_lastProcessTime;

// STATIC: 0x00D31A60
static SCritSect s_wardenCritSect;

// VERIFY: c0002692 auto
// OFFSET: 0x007DA2C0
static void UnloadPendingModule() {
    if (s_pendingModule) {
        s_pendingModule->Unload();

        BLLLoader* module = s_pendingModule;

        if (module) {
            module->~BLLLoader();
            SMemFree(module, "delete", SLOG_FUNCTION, 0);
        }

        s_pendingModule = nullptr;
    }
}

// VERIFY: 1f0c3c6a auto
// OFFSET: 0x007DA300
static void UnloadActiveModule() {
    if (s_activeModule) {
        s_moduleInterface->Shutdown();
        s_activeModule->Unload();

        BLLLoader* module = s_activeModule;

        if (module) {
            module->~BLLLoader();
            SMemFree(module, "delete", SLOG_FUNCTION, 0);
        }

        s_activeModule = nullptr;
        s_moduleInterface = nullptr;
    }
}

// VERIFY: ef688b60 auto
// OFFSET: 0x007DA360
static bool ActivateNextModule() {
    if (!s_pendingModule) {
        return false;
    }

    UnloadActiveModule();

    WARDENMODULEEXPORT exportFunc = reinterpret_cast<WARDENMODULEEXPORT>(s_pendingModule->GetExport(1));

    if (exportFunc) {
        s_moduleInterface = exportFunc(&s_ourLibrary);

        if (s_moduleInterface) {
            s_moduleInterface->Initialize(ClientServices::Connection()->GetSessionKey(), sizeof(LoginData::m_sessionKey));
            s_activeModule = s_pendingModule;
            s_pendingModule = nullptr;

            return true;
        }
    }

    UnloadPendingModule();

    return false;
}

// VERIFY: eed4f948 auto
// OFFSET: 0x007DA3D0
void WardenClient_Process() {
    s_wardenCritSect.Enter();

    uint32_t now = OsGetAsyncTimeMs();
    uint32_t elapsed = now - s_lastProcessTime;
    s_lastProcessTime = now;

    if (s_activeModule) {
        s_moduleInterface->Process(elapsed);
        ActivateNextModule();
    }

    s_wardenCritSect.Leave();
}

// INLINED
void WardenCachedModule::Pack(CDataStore* msg) {
}

#endif
