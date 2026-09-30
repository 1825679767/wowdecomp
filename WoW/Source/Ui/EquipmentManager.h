#ifndef WOW_SOURCE_UI_EQUIPMENTMANAGER_H
#define WOW_SOURCE_UI_EQUIPMENTMANAGER_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"

class CDataStore;
struct lua_State;

struct EQUIPMENT_SET {
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ int32_t m_id;
    /* 0014 */ char m_icon[256];
    /* 0114 */ char m_name[132];
    /* 0198 */ WOWGUID m_items[19];
    /* 0230 */ uint32_t unk_0230;
};

int32_t Packet_SMSG_EQUIPMENT_SET_LIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_EQUIPMENT_SET_SAVED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_EQUIPMENT_SET_USE_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

class CGEquipmentManager {
    public:
    static uint32_t* s_equipmentSetHeap;
    static void EnterWorld();
    static uint8_t m_lastCanUseEquipmentSetsValue;
    static uint8_t m_swapPending;
    static int32_t m_ignoredSlots[19];
    static int32_t m_pendingSetID;
    static int32_t CanUseEquipmentSets(bool a1);
    static const EQUIPMENT_SET* GetSetByID(int32_t id);
    static int32_t SetContainsLockedItems(const EQUIPMENT_SET* set);
    static int32_t SetUsesLockedSlots(const EQUIPMENT_SET* set);
    static void InitializeGame();
    static void EquipSet(int32_t id);
    static void GetFullIconPath(int32_t id, char* buffer, uint32_t size);
};

int32_t Script_SaveEquipmentSet(lua_State* L);
int32_t Script_DeleteEquipmentSet(lua_State* L);
int32_t Script_RenameEquipmentSet(lua_State* L);
int32_t Script_EquipmentManagerClearIgnoredSlotsForSave(lua_State* L);
int32_t Script_GetEquipmentSetLocations(lua_State* L);
int32_t Script_GetEquipmentSetItemIDs(lua_State* L);
int32_t Script_GetNumEquipmentSets(lua_State* L);
int32_t Script_GetEquipmentSetInfo(lua_State* L);
int32_t Script_GetEquipmentSetInfoByName(lua_State* L);
int32_t Script_EquipmentSetContainsLockedItems(lua_State* L);
int32_t Script_PickupEquipmentSetByName(lua_State* L);
int32_t Script_PickupEquipmentSet(lua_State* L);
int32_t Script_UseEquipmentSet(lua_State* L);
void EquipmentManagerRegisterScriptFunctions();

#endif
