#include "MerchantFrame.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00BFA3F0
int32_t CGMerchantInfo::m_numMerchantItems;

// STATIC: 0x00BFA3F4
uint32_t CGMerchantInfo::m_numBuybackItems;

// STATIC: 0x00BF9128
MerchantItem CGMerchantInfo::m_items[150];

// VERIFY: 4b63a9a7 manual
// OFFSET: 0x00584150
MerchantItem* CGMerchantInfo::FindItemById(int32_t itemId) {
    if (!CGMerchantInfo::m_merchant || !CGMerchantInfo::m_numMerchantItems) {
        return nullptr;
    }

    for (uint32_t i = 0; i < sizeof(CGMerchantInfo::m_items) / sizeof(CGMerchantInfo::m_items[0]); i++) {
        if (CGMerchantInfo::m_items[i].m_itemID == itemId) {
            return &CGMerchantInfo::m_items[i];
        }
    }

    return nullptr;
}

// VERIFY: 7160c52f manual
// OFFSET: 0x00584B20
int32_t CGMerchantInfo::GetRepairCost(CGItem_C* item) {
    float multiplier = 1.0f;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* merchant = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGMerchantInfo::m_merchant, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (player && merchant) {
        multiplier = 1.0f - merchant->GetPlayerDiscount(player);
    }

    return CMath::fint_(static_cast<float>(static_cast<uint32_t>(item->GetRepairCost())) * multiplier);
}
