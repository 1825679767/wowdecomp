#ifndef WOW_SOURCE_UI_BATTLEFIELDINFO_H
#define WOW_SOURCE_UI_BATTLEFIELDINFO_H

#include <cstddef>
#include <cstdint>

class CGBattlefieldInfo {
    public:
    static uint64_t m_arenaOpponentPets[5][2];
    static uint64_t m_arenaOpponents[5];
};

uint32_t GetArenaPetGUIDCount();

#endif
