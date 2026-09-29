#ifndef WOW_SOURCE_UI_CONTAINERFRAME_H
#define WOW_SOURCE_UI_CONTAINERFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGContainerInfo {
    public:
    static WOWGUID m_containers[11];
    static void UpdateContainers();
    static void EnterWorld();
    static void UpdateCooldowns();
    static void UpdateItem(WOWGUID guid);
};

int32_t Script_ContainerIDToInventoryID(lua_State* L);
int32_t Script_GetContainerNumSlots(lua_State* L);
int32_t Script_GetContainerItemInfo(lua_State* L);
int32_t Script_GetContainerItemID(lua_State* L);
int32_t Script_GetContainerItemLink(lua_State* L);
int32_t Script_GetContainerItemCooldown(lua_State* L);
int32_t Script_PickupContainerItem(lua_State* L);
int32_t Script_SplitContainerItem(lua_State* L);
int32_t Script_UseContainerItem(lua_State* L);
int32_t Script_SocketContainerItem(lua_State* L);
int32_t Script_ShowContainerSellCursor(lua_State* L);
int32_t Script_SetBagPortraitTexture(lua_State* L);
int32_t Script_GetBagName(lua_State* L);
int32_t Script_GetContainerItemDurability(lua_State* L);
int32_t Script_GetContainerNumFreeSlots(lua_State* L);
int32_t Script_GetContainerFreeSlots(lua_State* L);
int32_t Script_GetContainerItemPurchaseInfo(lua_State* L);
int32_t Script_GetContainerItemPurchaseItem(lua_State* L);
int32_t Script_ContainerRefundItemPurchase(lua_State* L);
int32_t Script_GetMaxArenaCurrency(lua_State* L);
int32_t Script_GetContainerItemGems(lua_State* L);
int32_t Script_GetContainerItemQuestInfo(lua_State* L);
void ContainerRegisterScriptFunctions();

int32_t InvUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t UpdateInvContents(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

#endif
