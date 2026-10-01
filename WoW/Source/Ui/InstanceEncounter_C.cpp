#include "InstanceEncounter_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C242FC
int32_t CGInstanceEncounter_C::m_units;

// STATIC: 0x00C24300
InstanceEncounterUnit* CGInstanceEncounter_C::m_unitList;

// VERIFY: 91f190f2 manual
// OFFSET: 0x005ED710
WOWGUID CGInstanceEncounter_C::GetBoss(uint32_t index) {
    if (static_cast<int32_t>(index) < 0 || static_cast<int32_t>(index) >= CGInstanceEncounter_C::m_units) {
        return WOWGUID{ 0 };
    }

    return CGInstanceEncounter_C::m_unitList[index].m_guid;
}

// VERIFY: 3f43a707 auto
// OFFSET: 0x005ED740
uint32_t CGInstanceEncounter_C::GetNumBossSlots() {
    return CGInstanceEncounter_C::m_units;
}

// VERIFY: e1fb6f4f auto
// OFFSET: 0x005EDED0
void CGInstanceEncounter_C::EnterWorld() {
    if (!CGGameUI::m_currentlyReloadingUI) {
        ClientServices::SetMessageHandler(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, Packet_SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, nullptr);
    }
}
