#include "Spell_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AF51C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "SpellIsTargeting", &Script_SpellIsTargeting },
    { "SpellCanTargetItem", &Script_SpellCanTargetItem },
    { "SpellTargetItem", &Script_SpellTargetItem },
    { "SpellCanTargetUnit", &Script_SpellCanTargetUnit },
    { "SpellTargetUnit", &Script_SpellTargetUnit },
    { "SpellCanTargetGlyph", &Script_SpellCanTargetGlyph },
    { "SpellStopTargeting", &Script_SpellStopTargeting },
    { "SpellStopCasting", &Script_SpellStopCasting },
    { "CancelUnitBuff", &Script_CancelUnitBuff },
    { "CancelItemTempEnchantment", &Script_CancelItemTempEnchantment },
    { "CannotBeResurrected", &Script_CannotBeResurrected },
};

// VERIFY: c99a9227 auto
// OFFSET: 0x007FDDB0
void SpellRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
