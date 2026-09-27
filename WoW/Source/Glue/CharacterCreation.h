#ifndef WOW_SOURCE_GLUE_CHARACTERCREATION_H
#define WOW_SOURCE_GLUE_CHARACTERCREATION_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleModelFFX;

class CCharacterCreation {
    public:
    static uint8_t m_charPreferences[176];
    static CSimpleModelFFX* m_charCustomizeFrame;
    static float m_charFacing;
    static TSGrowableArray<int32_t> m_races;
    static int32_t m_existingCharacterIndex;

    static int32_t Idle(const void* data, void* param);
    static void Initialize();
    static void OnHide();
};

void CharacterCreateRegisterScriptFunctions();

#endif
