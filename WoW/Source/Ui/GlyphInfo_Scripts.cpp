#include "GlyphInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACFF54
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumGlyphSockets", &Script_GetNumGlyphSockets },
    { "GetGlyphSocketInfo", &Script_GetGlyphSocketInfo },
    { "GlyphMatchesSocket", &Script_GlyphMatchesSocket },
    { "PlaceGlyphInSocket", &Script_PlaceGlyphInSocket },
    { "RemoveGlyphFromSocket", &Script_RemoveGlyphFromSocket },
    { "GetGlyphLink", &Script_GetGlyphLink },
};

// VERIFY: 177745e8 auto
// OFFSET: 0x005B7210
void GlyphInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
