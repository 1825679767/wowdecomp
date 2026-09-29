#ifndef WOW_SOURCE_UI_BATTLEFIELDINFO_H
#define WOW_SOURCE_UI_BATTLEFIELDINFO_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGBattlefieldInfo {
    public:
    static WOWGUID m_arenaOpponentPets[5][2];
    static WOWGUID m_arenaOpponents[5];
    static int32_t m_instanceType;
    static uint8_t m_arenaFaction;
    static int32_t m_arenaOpponentSeen[5];

    static void AddArenaOpponent(const WOWGUID& guid);
    static void AddVehicle(const WOWGUID& guid);
    static void AddArenaOpponentPet(const WOWGUID& petGuid, const WOWGUID& ownerGuid);
    static int32_t GetArenaOpponentIndex(const WOWGUID& guid, int32_t* index);
    static void RemoveVehicle(const WOWGUID& guid);
};

uint32_t GetArenaPetGUIDCount();

#endif
