#include "ItemTextFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACEEC0
static FrameScript_Method s_ScriptFunctions[] = {
    { "ItemTextGetItem", &Script_ItemTextGetItem },
    { "ItemTextGetCreator", &Script_ItemTextGetCreator },
    { "ItemTextGetMaterial", &Script_ItemTextGetMaterial },
    { "ItemTextGetPage", &Script_ItemTextGetPage },
    { "ItemTextGetText", &Script_ItemTextGetText },
    { "ItemTextHasNextPage", &Script_ItemTextHasNextPage },
    { "ItemTextPrevPage", &Script_ItemTextPrevPage },
    { "ItemTextNextPage", &Script_ItemTextNextPage },
    { "CloseItemText", &Script_CloseItemText },
};

// VERIFY: 65411387 auto
// OFFSET: 0x00589BB0
void ItemTextRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
