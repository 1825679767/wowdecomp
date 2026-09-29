#include "Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGBankInfo.h"

// VERIFY: 578d255a manual
// OFFSET: 0x00754040
int32_t CGBag_C::FindItemSlot(const CGItem_C* item) const {
    if (!item) {
        return -1;
    }

    for (uint32_t slot = 0; slot < static_cast<uint32_t>(this->m_numSlots); slot++) {
        WOWGUID guid = slot < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();

        if (ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__) == item) {
            return slot;
        }
    }

    return -1;
}

// VERIFY: 121306f8 manual
// OFFSET: 0x00754390
const CGItem_C* CGBag_C::GetItemPointer(int32_t slot) const {
    if ((this->m_hasBank && ((slot >= 39 && slot <= 66) || static_cast<uint32_t>(slot - 67) <= 6) && !CGBankInfo::m_banker) || slot >= this->m_numSlots) {
        return nullptr;
    }

    WOWGUID guid = static_cast<uint32_t>(slot) < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();

    return static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
}

// VERIFY: 6a3b7079 manual
// OFFSET: 0x00754400
CGItem_C* CGBag_C::GetItemPointer(int32_t slot) {
    if ((this->m_hasBank && ((slot >= 39 && slot <= 66) || static_cast<uint32_t>(slot - 67) <= 6) && !CGBankInfo::m_banker) || slot >= this->m_numSlots) {
        return nullptr;
    }

    WOWGUID guid = static_cast<uint32_t>(slot) < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();

    return static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));
}

// VERIFY: 8829b660 manual
// OFFSET: 0x00754470
int32_t CGBag_C::GetNumFreeInvSlots() {
    int32_t freeSlots = 0;

    for (uint32_t slot = 0; slot < static_cast<uint32_t>(this->m_numSlots); slot++) {
        if (this->m_hasBank) {
            if (slot <= 18 || (slot >= 39 && slot <= 66) || (slot >= 67 && slot <= 73) || (slot >= 86 && slot <= 117) || (slot >= 74 && slot <= 85) || slot - 118 <= 31) {
                continue;
            }

            if (slot - 19 <= 3) {
                WOWGUID guid = slot < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item && item->GetClassID() == 1) {
                    freeSlots += item->GetBag()->GetNumFreeInvSlots();
                }

                continue;
            }
        }

        if (slot >= static_cast<uint32_t>(this->m_numSlots) || !this->m_slots[slot]) {
            freeSlots++;
        }
    }

    return freeSlots;
}

// VERIFY: 8d4f2769 auto
// OFFSET: 0x00754560
int32_t GetItemTypeCountCallback(const CGItem_C* item, void* param) {
    ItemTypeCountParam* count = static_cast<ItemTypeCountParam*>(param);

    if (static_cast<int32_t>(item->m_obj->m_entryID) == count->m_itemId || count->m_itemId == -1) {
        count->m_count += item->m_item->m_itemFieldStackCount;
    }

    return 0;
}

// VERIFY: af9218e7 manual
// OFFSET: 0x00754940
uint32_t CGBag_C::GetAvgEquippedDurability() const {
    int32_t durability = 0;
    int32_t maxDurability = 0;

    for (uint32_t slot = 0; slot <= 18; slot++) {
        if ((this->m_hasBank && ((slot >= 39 && slot <= 66) || slot - 67 <= 6) && !CGBankInfo::m_banker) || static_cast<int32_t>(slot) >= this->m_numSlots) {
            continue;
        }

        WOWGUID guid = slot < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item) {
            continue;
        }

        durability += (item->m_item->m_itemFieldFlags >> 3) & 1 ? 0 : item->m_item->m_itemFieldDurability;
        maxDurability += (item->m_item->m_itemFieldFlags >> 3) & 1 ? 0 : item->m_item->m_itemFieldMaxdurability;
    }

    if (!maxDurability) {
        return 0;
    }

    return static_cast<float>(durability) / maxDurability * 100.0f;
}
