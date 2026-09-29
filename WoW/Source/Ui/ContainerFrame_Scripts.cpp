#include "ContainerFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

// STATIC: 0x00AD0D50
static FrameScript_Method s_ScriptFunctions[] = {
    { "ContainerIDToInventoryID", &Script_ContainerIDToInventoryID },
    { "GetContainerNumSlots", &Script_GetContainerNumSlots },
    { "GetContainerItemInfo", &Script_GetContainerItemInfo },
    { "GetContainerItemID", &Script_GetContainerItemID },
    { "GetContainerItemLink", &Script_GetContainerItemLink },
    { "GetContainerItemCooldown", &Script_GetContainerItemCooldown },
    { "PickupContainerItem", &Script_PickupContainerItem },
    { "SplitContainerItem", &Script_SplitContainerItem },
    { "UseContainerItem", &Script_UseContainerItem },
    { "SocketContainerItem", &Script_SocketContainerItem },
    { "ShowContainerSellCursor", &Script_ShowContainerSellCursor },
    { "SetBagPortraitTexture", &Script_SetBagPortraitTexture },
    { "GetBagName", &Script_GetBagName },
    { "GetContainerItemDurability", &Script_GetContainerItemDurability },
    { "GetContainerNumFreeSlots", &Script_GetContainerNumFreeSlots },
    { "GetContainerFreeSlots", &Script_GetContainerFreeSlots },
    { "GetContainerItemPurchaseInfo", &Script_GetContainerItemPurchaseInfo },
    { "GetContainerItemPurchaseItem", &Script_GetContainerItemPurchaseItem },
    { "ContainerRefundItemPurchase", &Script_ContainerRefundItemPurchase },
    { "GetMaxArenaCurrency", &Script_GetMaxArenaCurrency },
    { "GetContainerItemGems", &Script_GetContainerItemGems },
    { "GetContainerItemQuestInfo", &Script_GetContainerItemQuestInfo },
};

// VERIFY: 1248b250 auto
// OFFSET: 0x005D7020
void ContainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}
