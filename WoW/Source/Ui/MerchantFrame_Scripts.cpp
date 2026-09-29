#include "MerchantFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00ACED00
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseMerchant", &Script_CloseMerchant },
    { "GetMerchantNumItems", &Script_GetMerchantNumItems },
    { "GetMerchantItemInfo", &Script_GetMerchantItemInfo },
    { "GetMerchantItemCostInfo", &Script_GetMerchantItemCostInfo },
    { "GetMerchantItemCostItem", &Script_GetMerchantItemCostItem },
    { "GetBuybackItemInfo", &Script_GetBuybackItemInfo },
    { "GetBuybackItemLink", &Script_GetBuybackItemLink },
    { "GetMerchantItemLink", &Script_GetMerchantItemLink },
    { "GetMerchantItemMaxStack", &Script_GetMerchantItemMaxStack },
    { "PickupMerchantItem", &Script_PickupMerchantItem },
    { "BuyMerchantItem", &Script_BuyMerchantItem },
    { "BuybackItem", &Script_BuybackItem },
    { "CanMerchantRepair", &Script_CanMerchantRepair },
    { "ShowMerchantSellCursor", &Script_ShowMerchantSellCursor },
    { "ShowBuybackSellCursor", &Script_ShowBuybackSellCursor },
    { "ShowRepairCursor", &Script_ShowRepairCursor },
    { "HideRepairCursor", &Script_HideRepairCursor },
    { "InRepairMode", &Script_InRepairMode },
    { "GetRepairAllCost", &Script_GetRepairAllCost },
    { "RepairAllItems", &Script_RepairAllItems },
    { "GetNumBuybackItems", &Script_GetNumBuybackItems },
};

// VERIFY: 47ef76cb auto
// OFFSET: 0x00584450
void MerchantRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
