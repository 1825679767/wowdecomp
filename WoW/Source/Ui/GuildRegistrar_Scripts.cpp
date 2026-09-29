#include "GuildRegistrar.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF5FC
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseGuildRegistrar", &Script_CloseGuildRegistrar },
    { "GetGuildCharterCost", &Script_GetGuildCharterCost },
    { "BuyGuildCharter", &Script_BuyGuildCharter },
    { "TurnInGuildCharter", &Script_TurnInGuildCharter },
    { "GetTabardInfo", &Script_GetTabardInfo },
};

// VERIFY: e5b8e04b auto
// OFFSET: 0x00599F80
void GuildRegistrarRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
