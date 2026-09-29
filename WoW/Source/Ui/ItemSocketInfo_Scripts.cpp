#include "ItemSocketInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0568
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseSocketInfo", &Script_CloseSocketInfo },
    { "GetSocketItemInfo", &Script_GetSocketItemInfo },
    { "GetNumSockets", &Script_GetNumSockets },
    { "GetExistingSocketInfo", &Script_GetExistingSocketInfo },
    { "GetExistingSocketLink", &Script_GetExistingSocketLink },
    { "GetNewSocketInfo", &Script_GetNewSocketInfo },
    { "GetNewSocketLink", &Script_GetNewSocketLink },
    { "ClickSocketButton", &Script_ClickSocketButton },
    { "AcceptSockets", &Script_AcceptSockets },
    { "GetSocketTypes", &Script_GetSocketTypes },
    { "GetSocketItemRefundable", &Script_GetSocketItemRefundable },
    { "GetSocketItemBoundTradeable", &Script_GetSocketItemBoundTradeable },
};

// VERIFY: 16788a4d auto
// OFFSET: 0x005C4630
void ItemSocketInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
