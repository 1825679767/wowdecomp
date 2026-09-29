#include "TaxiMapFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACF258
static FrameScript_Method s_ScriptFunctions[] = {
    { "SetTaxiMap", &Script_SetTaxiMap },
    { "NumTaxiNodes", &Script_NumTaxiNodes },
    { "TaxiNodeName", &Script_TaxiNodeName },
    { "TaxiNodePosition", &Script_TaxiNodePosition },
    { "TaxiNodeCost", &Script_TaxiNodeCost },
    { "TakeTaxiNode", &Script_TakeTaxiNode },
    { "CloseTaxiMap", &Script_CloseTaxiMap },
    { "TaxiNodeGetType", &Script_TaxiNodeGetType },
    { "TaxiNodeSetCurrent", &Script_TaxiNodeSetCurrent },
    { "TaxiGetSrcX", &Script_TaxiGetSrcX },
    { "TaxiGetSrcY", &Script_TaxiGetSrcY },
    { "TaxiGetDestX", &Script_TaxiGetDestX },
    { "TaxiGetDestY", &Script_TaxiGetDestY },
    { "GetNumRoutes", &Script_GetNumRoutes },
};

// VERIFY: 8e9fe376 auto
// OFFSET: 0x00590750
void CGTaxiMap::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
