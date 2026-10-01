#include "EquipmentManager.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include <new>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include <cstring>
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C1E95C
uint32_t* CGEquipmentManager::s_equipmentSetHeap;

// STATIC: 0x00C1E954
uint8_t CGEquipmentManager::m_lastCanUseEquipmentSetsValue;

// STATIC: 0x00C1E955
uint8_t CGEquipmentManager::m_loaded;

// STATIC: 0x00C1E956
uint8_t CGEquipmentManager::m_swapPending;

// STATIC: 0x00C1E958
int32_t CGEquipmentManager::m_pendingSetID;

// STATIC: 0x00C1E908
int32_t CGEquipmentManager::m_ignoredSlots[19];

// STATIC: 0x00ACFC00
TSList<EQUIPMENT_SET_NODE> CGEquipmentManager::s_equipmentSets;

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

// VERIFY: ecc66233 manual
// OFFSET: 0x005AE380
int32_t CGEquipmentManager::GetSetList(char* buffer, uint32_t size, const WOWGUID* item) {
    uint32_t remaining = size - 1;
    char* out = buffer;
    const char* separator = "";
    const char* delimiter = FrameScript_GetText("PLAYER_LIST_DELIMITER", -1, GENDER_NOT_APPLICABLE);

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        EQUIPMENT_SET* set = node->m_set;

        for (uint32_t i = 0; i < sizeof(set->m_items) / sizeof(set->m_items[0]); i++) {
            if (set->m_items[i] != *item) {
                continue;
            }

            size_t nameLength = SStrLen(set->m_name);
            size_t separatorLength = SStrLen(separator);

            if (separatorLength + nameLength > remaining) {
                break;
            }

            memcpy(out, separator, separatorLength);
            out += separatorLength;
            memcpy(out, set->m_name, nameLength);
            out += nameLength;
            remaining -= nameLength + separatorLength;
            separator = delimiter;
        }
    }

    *out = '\0';
    return buffer != out;
}

// VERIFY: b88be2b9 manual
// OFFSET: 0x005AE5C0
const EQUIPMENT_SET* CGEquipmentManager::GetSetByID(int32_t id) {
    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        if (node->m_set->m_id == id) {
            return node->m_set;
        }
    }

    return nullptr;
}

// INLINED
EQUIPMENT_SET::EQUIPMENT_SET() {
    this->m_guid = WOWGUID{ 0 };
    memset(this->m_items, 0, sizeof(this->m_items));
    this->unk_0230 = 0;
}

// VERIFY: 816e8bf2 manual
// OFFSET: 0x005AF490
int32_t Packet_SMSG_EQUIPMENT_SET_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count = 0;
    msg->Get(count);

    uint32_t existing = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); !CGEquipmentManager::s_equipmentSets.IsEnd(node); node = CGEquipmentManager::s_equipmentSets.RawNext(node)) {
        existing++;
    }

    if (CGEquipmentManager::m_loaded && count == existing) {
        msg->m_read = msg->m_size;
        return 1;
    }

    if (existing) {
        for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); !CGEquipmentManager::s_equipmentSets.IsEnd(node); node = CGEquipmentManager::s_equipmentSets.RawNext(node)) {
            ObjectFree(*CGEquipmentManager::s_equipmentSetHeap, node->m_set->m_handle);
        }

        CGEquipmentManager::s_equipmentSets.Clear();
    }

    for (int32_t i = 0; i < static_cast<int32_t>(count) && i < 10; i++) {
        EQUIPMENT_SET* set = nullptr;
        uint32_t handle;
        void* object = nullptr;

        if (ObjectAlloc(*CGEquipmentManager::s_equipmentSetHeap, &handle, &object, false)) {
            set = object ? new (object) EQUIPMENT_SET() : nullptr;
            set->m_handle = handle;
        }

        SmartGUID guid;
        *msg >> guid;
        set->m_guid = guid;
        msg->Get(reinterpret_cast<uint32_t&>(set->m_id));
        msg->GetString(set->m_name, sizeof(set->m_name));
        msg->GetString(set->m_icon, sizeof(set->m_icon));

        for (uint32_t slot = 0; slot < 19; slot++) {
            SmartGUID item;
            *msg >> item;
            set->m_items[slot] = item;
        }

        EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.NewNode(2, 0, 0);
        node->m_set = set;
    }

    CGEquipmentManager::m_loaded = 1;
    return 1;
}

// VERIFY: 04c7f51f manual
// OFFSET: 0x005AE1B0
void CGEquipmentManager::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_LOAD_EQUIPMENT_SET);
    ClientServices::ClearMessageHandler(SMSG_EQUIPMENT_SET_ID);
    ClientServices::ClearMessageHandler(SMSG_USE_EQUIPMENT_SET_RESULT);
    CGEquipmentManager::m_swapPending = 0;
    CGEquipmentManager::m_pendingSetID = 0;
    CGEquipmentManager::m_loaded = 0;

    for (EQUIPMENT_SET_NODE* node = CGEquipmentManager::s_equipmentSets.Head(); node; node = CGEquipmentManager::s_equipmentSets.Next(node)) {
        ObjectFree(*CGEquipmentManager::s_equipmentSetHeap, node->m_set->m_handle);
    }

    CGEquipmentManager::s_equipmentSets.Clear();
    delete CGEquipmentManager::s_equipmentSetHeap;
    CGEquipmentManager::s_equipmentSetHeap = nullptr;
}
