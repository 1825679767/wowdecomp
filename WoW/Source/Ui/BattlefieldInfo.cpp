#include "BattlefieldInfo.h"

// STATIC: 0x00BE9EF8
WOWGUID CGBattlefieldInfo::m_arenaOpponentPets[5][2];

// STATIC: 0x00BE9F48
WOWGUID CGBattlefieldInfo::m_arenaOpponents[5];

// VERIFY: 39950e3a auto
// OFFSET: 0x00549630
uint32_t GetArenaPetGUIDCount() {
    uint32_t count = 0;

    do {
        if (!CGBattlefieldInfo::m_arenaOpponents[count] && !CGBattlefieldInfo::m_arenaOpponentPets[count][0]) {
            break;
        }

        count++;
    } while (count < 5);

    return count;
}
