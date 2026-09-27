#include <cstdint>
#include "Client.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: 0x00401B00
int32_t CCommand_ReloadUI(const char* command, const char* arguments) {
    CGlueMgr::m_reload = 1;
    CGGameUI::Reload();
    return 1;
}

// VERIFY: d6492c90 auto
// OFFSET: 0x008C8DE0
int32_t CCommand_Perf(const char* command, const char* arguments) {
    return 1;
}

// VERIFY: c68202b2 auto
// OFFSET: 0x004032A0
int32_t CCommand_TimingInfo(const char* command, const char* arguments) {
    int32_t desired = CVar::LookupRegistered("timingMethod")->m_intValue;
    OsTimeManager::TimingMethod selected = OsTimeGetTimingMethod();
    int32_t testError = CVar::LookupRegistered("timingTestError")->m_intValue;

    ConsolePrintf("Timing method desired: %d - %s", desired, GetTimingMethodName(static_cast<OsTimeManager::TimingMethod>(desired)));
    ConsolePrintf("Timing method selected: %d - %s", selected, GetTimingMethodName(selected));
    ConsolePrintf("Timing test error: %d", testError);

    return 1;
}
