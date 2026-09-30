#ifndef WOW_SOURCE_UI_INSTANCEENCOUNTER_C_H
#define WOW_SOURCE_UI_INSTANCEENCOUNTER_C_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct InstanceEncounterUnit {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t unk_0008[2];
};

class CGInstanceEncounter_C {
    public:
    static void EnterWorld();
    static WOWGUID GetBoss(uint32_t index);
    static uint32_t GetNumBossSlots();
    static int32_t m_units;
    static InstanceEncounterUnit* m_unitList;
};

#endif
