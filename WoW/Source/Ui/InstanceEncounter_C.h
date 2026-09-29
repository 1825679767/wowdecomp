#ifndef WOW_SOURCE_UI_INSTANCEENCOUNTER_C_H
#define WOW_SOURCE_UI_INSTANCEENCOUNTER_C_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGInstanceEncounter_C {
    public:
    static void EnterWorld();
    static WOWGUID GetBoss(uint32_t index);
};

#endif
