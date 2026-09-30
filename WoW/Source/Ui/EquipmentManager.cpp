#include "EquipmentManager.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include <new>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C1E95C
uint32_t* CGEquipmentManager::s_equipmentSetHeap;

// STATIC: 0x00C1E954
uint8_t CGEquipmentManager::m_lastCanUseEquipmentSetsValue;

// STATIC: 0x00C1E956
uint8_t CGEquipmentManager::m_swapPending;

// STATIC: 0x00C1E958
int32_t CGEquipmentManager::m_pendingSetID;

// STATIC: 0x00C1E908
int32_t CGEquipmentManager::m_ignoredSlots[19];

// VERIFY: 36e16d6a auto
// OFFSET: 0x005AD690
void CGEquipmentManager::EnterWorld() {
    CGEquipmentManager::m_swapPending = 0;
    CGEquipmentManager::m_lastCanUseEquipmentSetsValue = 0;
    CGEquipmentManager::m_pendingSetID = 0;
}

// VERIFY: 6b01312b auto
// OFFSET: 0x005AD6B0
int32_t CGEquipmentManager::CanUseEquipmentSets(bool a1) {
    uint8_t canUse = 0;

    if (!CGEquipmentManager::m_swapPending) {
        CVar* var = CVar::Lookup("equipmentManager");

        if (var && var->m_intValue) {
            canUse = 1;
        }
    }

    if (CGEquipmentManager::m_lastCanUseEquipmentSetsValue != canUse) {
        CGEquipmentManager::m_lastCanUseEquipmentSetsValue = canUse;

        if (a1) {
            CGActionBar::UpdateUsable();
        }
    }

    return canUse;
}

// VERIFY: ca2761b6 manual
// OFFSET: 0x005AFBB0
void CGEquipmentManager::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_LOAD_EQUIPMENT_SET, Packet_SMSG_EQUIPMENT_SET_LIST, nullptr);
    ClientServices::SetMessageHandler(SMSG_EQUIPMENT_SET_ID, Packet_SMSG_EQUIPMENT_SET_SAVED, nullptr);
    ClientServices::SetMessageHandler(SMSG_USE_EQUIPMENT_SET_RESULT, Packet_SMSG_EQUIPMENT_SET_USE_RESULT, nullptr);

    if (!CGEquipmentManager::s_equipmentSetHeap) {
        CGEquipmentManager::s_equipmentSetHeap = new (SMemAlloc(sizeof(uint32_t), __FILE__, __LINE__, 0)) uint32_t(ObjectAllocAddHeap(sizeof(EQUIPMENT_SET), 128, "EQUIPMENTSET", true));
    }
}
