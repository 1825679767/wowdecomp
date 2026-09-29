#ifndef WOW_SOURCE_UI_PAPERDOLLINFOFRAME_H
#define WOW_SOURCE_UI_PAPERDOLLINFOFRAME_H

#include <cstdint>
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGPlayer_C;

class CGCharacterInfo {
    public:
    static TSFixedArray<int32_t> m_invAlertStatus;
    static int32_t CalculateInventoryAlertStatus(CGPlayer_C* player, uint32_t slot);
    static void RegisterMirrorHandlers(WOWGUID guid);
    static WOWGUID m_inspectTarget;
    static int32_t CanInspect(const CGPlayer_C* unit);
    static int32_t GetActivePlayerAmmoID(const CGPlayer_C* player);
    static void EnterWorld();
    static void InitializeGame();
    static void UpdateInventoryAlerts();
    static void UpdateItem(WOWGUID guid);
};

int32_t Script_GetInventorySlotInfo(lua_State* L);
int32_t Script_GetInventoryItemsForSlot(lua_State* L);
int32_t Script_GetInventoryItemTexture(lua_State* L);
int32_t Script_GetInventoryItemBroken(lua_State* L);
int32_t Script_GetInventoryItemCount(lua_State* L);
int32_t Script_GetInventoryItemQuality(lua_State* L);
int32_t Script_GetInventoryItemCooldown(lua_State* L);
int32_t Script_GetInventoryItemDurability(lua_State* L);
int32_t Script_GetInventoryItemLink(lua_State* L);
int32_t Script_GetInventoryItemID(lua_State* L);
int32_t Script_GetInventoryItemGems(lua_State* L);
int32_t Script_KeyRingButtonIDToInvSlotID(lua_State* L);
int32_t Script_PickupInventoryItem(lua_State* L);
int32_t Script_UseInventoryItem(lua_State* L);
int32_t Script_SocketInventoryItem(lua_State* L);
int32_t Script_IsInventoryItemLocked(lua_State* L);
int32_t Script_PutItemInBag(lua_State* L);
int32_t Script_PutItemInBackpack(lua_State* L);
int32_t Script_PickupBagFromSlot(lua_State* L);
int32_t Script_CursorCanGoInSlot(lua_State* L);
int32_t Script_ShowInventorySellCursor(lua_State* L);
int32_t Script_SetInventoryPortraitTexture(lua_State* L);
int32_t Script_GetGuildInfo(lua_State* L);
int32_t Script_GetInventoryAlertStatus(lua_State* L);
int32_t Script_UpdateInventoryAlertStatus(lua_State* L);
int32_t Script_OffhandHasWeapon(lua_State* L);
int32_t Script_HasInspectHonorData(lua_State* L);
int32_t Script_RequestInspectHonorData(lua_State* L);
int32_t Script_GetInspectHonorData(lua_State* L);
int32_t Script_GetInspectArenaTeamData(lua_State* L);
int32_t Script_ClearInspectPlayer(lua_State* L);
int32_t Script_GetWeaponEnchantInfo(lua_State* L);
int32_t Script_HasWandEquipped(lua_State* L);
void CharacterInfoRegisterScriptFunctions();

class CDataStore;

int32_t Packet_MSG_INSPECT_HONOR_STATS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_INSPECT_ARENA_TEAMS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

int32_t PlayerCharacterPointsUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t PlayerEquipmentChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

int32_t ResolveInventorySlotArg(lua_State* L, int32_t index, int32_t& slot);
int32_t GetUnitAndSlotFromLua(lua_State* L, const char*& unit, int32_t& slot);
void UnitInventoryChangedCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

#endif
