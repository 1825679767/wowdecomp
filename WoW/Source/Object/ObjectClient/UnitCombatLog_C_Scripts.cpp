#include "UnitCombatLog_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ADB8E0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CombatLogResetFilter", &Script_CombatLogResetFilter },
    { "CombatLogAddFilter", &Script_CombatLogAddFilter },
    { "CombatLogSetRetentionTime", &Script_CombatLogSetRetentionTime },
    { "CombatLogGetRetentionTime", &Script_CombatLogGetRetentionTime },
    { "CombatLogGetNumEntries", &Script_CombatLogGetNumEntries },
    { "CombatLogSetCurrentEntry", &Script_CombatLogSetCurrentEntry },
    { "CombatLogGetCurrentEntry", &Script_CombatLogGetCurrentEntry },
    { "CombatLogAdvanceEntry", &Script_CombatLogAdvanceEntry },
    { "CombatLogClearEntries", &Script_CombatLogClearEntries },
    { "CombatLog_Object_IsA", &Script_CombatLog_Object_IsA },
    { "CombatTextSetActiveUnit", &Script_CombatTextSetActiveUnit },
};

// VERIFY: 0abd2d5c auto
// OFFSET: 0x0074D6B0
void UnitCombatLogRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
