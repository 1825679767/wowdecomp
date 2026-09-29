#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_BAG_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_BAG_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CGItem_C;

struct ItemTypeCountParam {
    /* 0000 */ int32_t m_itemId;
    /* 0004 */ int32_t m_count;
};

class CGBag_C {
    public:
    /* 0000 */ int32_t m_numSlots;
    /* 0004 */ WOWGUID* m_slots;
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint8_t m_hasBank;

    int32_t FindItemSlot(const CGItem_C* item) const;
    uint32_t GetAvgEquippedDurability() const;
    const CGItem_C* GetItemPointer(int32_t slot) const;
    int32_t GetNumFreeInvSlots();
    CGItem_C* GetItemPointer(int32_t slot);
};

int32_t GetItemTypeCountCallback(const CGItem_C* item, void* param);

#endif
