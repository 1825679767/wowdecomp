#ifndef WOW_SOURCE_UI_EQUIPMENTMANAGER_H
#define WOW_SOURCE_UI_EQUIPMENTMANAGER_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

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

class CGEquipmentManager {
    public:
    static void EnterWorld();
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
int32_t Script_EquipmentManagerIgnoreSlotForSave(lua_State* L);
int32_t Script_EquipmentManagerIsSlotIgnoredForSave(lua_State* L);
int32_t Script_EquipmentManagerClearIgnoredSlotsForSave(lua_State* L);
int32_t Script_EquipmentManagerUnignoreSlotForSave(lua_State* L);
int32_t Script_GetEquipmentSetLocations(lua_State* L);
int32_t Script_GetEquipmentSetItemIDs(lua_State* L);
int32_t Script_GetNumEquipmentSets(lua_State* L);
int32_t Script_GetEquipmentSetInfo(lua_State* L);
int32_t Script_GetEquipmentSetInfoByName(lua_State* L);
int32_t Script_EquipmentSetContainsLockedItems(lua_State* L);
int32_t Script_PickupEquipmentSetByName(lua_State* L);
int32_t Script_PickupEquipmentSet(lua_State* L);
int32_t Script_UseEquipmentSet(lua_State* L);
int32_t Script_CanUseEquipmentSets(lua_State* L);
void EquipmentManagerRegisterScriptFunctions();

#endif
