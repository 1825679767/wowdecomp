#include "GossipInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEF78
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetGossipText", &Script_GetGossipText },
    { "GetNumGossipOptions", &Script_GetNumGossipOptions },
    { "GetNumGossipAvailableQuests", &Script_GetNumGossipAvailableQuests },
    { "GetNumGossipActiveQuests", &Script_GetNumGossipActiveQuests },
    { "GetGossipOptions", &Script_GetGossipOptions },
    { "GetGossipAvailableQuests", &Script_GetGossipAvailableQuests },
    { "GetGossipActiveQuests", &Script_GetGossipActiveQuests },
    { "SelectGossipOption", &Script_SelectGossipOption },
    { "SelectGossipAvailableQuest", &Script_SelectGossipAvailableQuest },
    { "SelectGossipActiveQuest", &Script_SelectGossipActiveQuest },
    { "CloseGossip", &Script_CloseGossip },
    { "ForceGossip", &Script_ForceGossip },
};

// VERIFY: 872f7321 auto
// OFFSET: 0x0058AA60
void GossipInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
