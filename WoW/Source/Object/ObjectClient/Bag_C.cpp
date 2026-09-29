#include "Bag_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGBankInfo.h"

// VERIFY: 94284adc manual
// OFFSET: 0x00589C00
int32_t CGBag_C::FindItemSlot(WOWGUID guid) const {
    for (uint32_t slot = 0; slot < static_cast<uint32_t>(this->m_numSlots); slot++) {
        if (this->m_slots[slot] == guid) {
            return slot;
        }
    }

    return -1;
}

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

// VERIFY: c1b9cf4f auto
// OFFSET: 0x005125B0
int32_t CGBag_C::GetNumItems() const {
    int32_t count = 0;

    for (int32_t i = 0; i < this->m_numSlots; i++) {
        if (this->m_slots[i]) {
            count++;
        }
    }

    return count;
}

struct BAGSLOTRANGE {
    /* 0000 */ uint32_t m_first;
    /* 0004 */ uint32_t m_last;
};

// STATIC: 0x00A37B10
static const BAGSLOTRANGE s_bagSlotRanges[8] = { { 23, 38 }, { 19, 22 }, { 86, 117 }, { 39, 66 }, { 67, 73 }, { 118, 149 }, { 0, 18 }, { 74, 85 } };

// VERIFY: 724d9331 auto
// OFFSET: 0x00753A50
static int32_t __stdcall SlotMatchesSearchFlags(uint32_t slot, uint32_t flags) {
    return (slot <= 18 && (flags & 0x1)) || (slot - 23 <= 15 && (flags & 0x4)) || (slot - 19 <= 3 && (flags & 0x2)) || (((slot >= 39 && slot <= 66) || slot - 67 <= 6) && (flags & 0x8)) || (slot - 86 <= 31 && (flags & 0x40)) || (slot - 118 <= 31 && (flags & 0x200));
}

// VERIFY: 9c2bc99e auto
// OFFSET: 0x00753F60
int32_t GetItemTypeChargesCallback(const CGItem_C* item, void* param) {
    ItemTypeCountParam* count = static_cast<ItemTypeCountParam*>(param);

    if ((static_cast<int32_t>(item->m_obj->m_entryID) == count->m_itemId || count->m_itemId == -1) && const_cast<CGItem_C*>(item)->GetEnchantmentSpell(0)) {
        int32_t charges = const_cast<CGItem_C*>(item)->GetCurrentSpellCharges();

        if (charges <= 0) {
            charges = -charges;
        }

        count->m_count += charges;
    }

    return 0;
}

// VERIFY: b4bf3df4 auto
// OFFSET: 0x00754020
int32_t FindItemIDCallback(const CGItem_C* item, void* param) {
    return static_cast<int32_t>(item->m_obj->m_entryID) == *static_cast<int32_t*>(param);
}

// VERIFY: 4e7fa037 manual
// OFFSET: 0x007541F0
bool CGBag_C::FindTotemCategory(int32_t type, int32_t& mask, bool recurse, uint32_t flags) {
    if (!(flags & 0x7)) {
        flags |= 0x7;
    }

    uint32_t slot = this->m_hasBank ? s_bagSlotRanges[0].m_first : 0;

    while (slot < static_cast<uint32_t>(this->m_numSlots)) {
        if (!this->m_hasBank || SlotMatchesSearchFlags(slot, flags)) {
            WOWGUID guid = slot < static_cast<uint32_t>(this->m_numSlots) ? this->m_slots[slot] : WOWGUID();
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && (!this->m_hasBank || slot < 74 || slot > 85)) {
                WOWGUID none;
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, nullptr, nullptr, false);

                if (stats && stats->m_totemCategory) {
                    TotemCategoryRec* category = g_totemCategoryDB.GetRecord(stats->m_totemCategory);

                    if (category && category->m_totemCategoryType == type) {
                        mask &= ~category->m_totemCategoryMask;

                        if (!mask) {
                            return true;
                        }
                    }
                }

                if (recurse && (item->m_obj->m_type & TYPEMASK_CONTAINER) && item->GetBag() && item->GetBag()->FindTotemCategory(type, mask, false, flags)) {
                    return true;
                }
            }
        }

        if (slot < static_cast<uint32_t>(this->m_numSlots)) {
            uint32_t i = 0;

            if (this->m_hasBank) {
                for (; i < sizeof(s_bagSlotRanges) / sizeof(s_bagSlotRanges[0]); i++) {
                    if (s_bagSlotRanges[i].m_last == slot) {
                        break;
                    }
                }
            }

            if (this->m_hasBank && i < sizeof(s_bagSlotRanges) / sizeof(s_bagSlotRanges[0])) {
                slot = i >= sizeof(s_bagSlotRanges) / sizeof(s_bagSlotRanges[0]) - 1 ? this->m_numSlots : s_bagSlotRanges[i + 1].m_first;
            } else {
                slot++;
            }
        } else {
            slot = this->m_numSlots;
        }
    }

    return false;
}

// VERIFY: 6ed45ee8 auto
// OFFSET: 0x007548F0
bool CGBag_C::FindTotemCategory(int32_t category, uint32_t flags) {
    TotemCategoryRec* rec = g_totemCategoryDB.GetRecord(category);

    if (!rec) {
        return false;
    }

    int32_t mask = rec->m_totemCategoryMask;
    return this->FindTotemCategory(rec->m_totemCategoryType, mask, true, flags);
}

// VERIFY: 12f34396 auto
// OFFSET: 0x00754A20
CGItem_C* CGBag_C::FindItemOfType(int32_t itemId, uint32_t flags) {
    WOWGUID container;
    uint32_t slot;
    return this->FindItem(FindItemIDCallback, &itemId, container, slot, flags);
}

// VERIFY: ef1d9110 auto
// OFFSET: 0x00754A50
CGItem_C* CGBag_C::FindItemOfType(int32_t itemId, WOWGUID& container, uint32_t& slot, uint32_t flags) {
    return this->FindItem(FindItemIDCallback, &itemId, container, slot, flags);
}

// VERIFY: 98f5a653 auto
// OFFSET: 0x00754D00
int32_t CGBag_C::GetItemTypeCount(int32_t itemId, uint32_t flags) {
    if (itemId == -2) {
        return this->GetNumFreeInvSlots();
    }

    ItemTypeCountParam param;
    param.m_itemId = itemId;
    param.m_count = 0;
    WOWGUID container;
    uint32_t slot;
    this->FindItem(GetItemTypeCountCallback, &param, container, slot, flags);
    return param.m_count;
}

// VERIFY: a7154eb9 auto
// OFFSET: 0x00754D50
int32_t CGBag_C::GetItemTypeCharges(int32_t itemId, uint32_t flags) {
    ItemTypeCountParam param;
    param.m_itemId = itemId;
    param.m_count = 0;
    WOWGUID container;
    uint32_t slot;
    this->FindItem(GetItemTypeChargesCallback, &param, container, slot, flags);
    return param.m_count;
}

// VERIFY: 0a0051b7 auto
// OFFSET: 0x00753AC0
int32_t FindItemClassCallback(const CGItem_C* item, void* param) {
    const uint32_t* classParam = static_cast<const uint32_t*>(param);
    return item->GetClassID() == classParam[0] && (!classParam[1] || ((1 << item->GetSubtypeID()) & classParam[1]));
}

// VERIFY: 4b4db806 auto
// OFFSET: 0x00754A80
CGItem_C* CGBag_C::FindItemOfClass(int32_t classId, uint32_t subclassMask, uint32_t flags) {
    uint32_t param[2];
    param[0] = classId;
    param[1] = subclassMask;
    WOWGUID container;
    uint32_t slot;
    return this->FindItem(FindItemClassCallback, param, container, slot, flags);
}
