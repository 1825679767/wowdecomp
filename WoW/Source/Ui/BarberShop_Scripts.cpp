#include "BarberShop.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACC790
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetBarberShopStyleInfo", &Script_GetBarberShopStyleInfo },
    { "SetNextBarberShopStyle", &Script_SetNextBarberShopStyle },
    { "GetBarberShopTotalCost", &Script_GetBarberShopTotalCost },
    { "ApplyBarberShopStyle", &Script_ApplyBarberShopStyle },
    { "CancelBarberShop", &Script_CancelBarberShop },
    { "GetHairCustomization", &Script_GetHairCustomization },
    { "GetFacialHairCustomization", &Script_GetFacialHairCustomization },
    { "BarberShopReset", &Script_BarberShopReset },
    { "CanAlterSkin", &Script_CanAlterSkin },
};

// VERIFY: 5397546f auto
// OFFSET: 0x0052E520
void CGBarberShop::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
