#ifndef WOW_SOURCE_GLUE_CHARACTERSELECTION_H
#define WOW_SOURCE_GLUE_CHARACTERSELECTION_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"

class CSimpleModelFFX;

class CCharacterSelection {
    public:
    static CSimpleModelFFX* m_modelFrame;
    static TSGrowableArray<CharacterSelectionDisplay> s_characterList;

    static void ClearCharacterList();
    static int32_t Idle(const void* data, void* param);
    static void Initialize();
    static void OnHide();
    static void UpdateCharacterList();
};

void CharSelectRegisterScriptFunctions();

#endif
