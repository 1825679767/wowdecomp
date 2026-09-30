#ifndef WOW_SOURCE_UI_LOOTFRAME_H
#define WOW_SOURCE_UI_LOOTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct LootInfo {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
};

void LootDestroy();
void LootInitialize();

struct LootSlot {
    /* 0000 */ int32_t m_pending;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ int32_t m_count;
    /* 0010 */ int32_t unk_0010;
    /* 0014 */ int32_t unk_0014;
    /* 0018 */ uint8_t m_slot;
    /* 0019 */ uint8_t unk_0019[3];
    /* 001C */ int32_t m_slotType;
};

class CGLootInfo {
    public:
    static uint32_t m_lootType;
    static LootSlot m_slots[18];
    static int32_t m_coins;
    static WOWGUID m_lootObj;

    static void InitializeGame();
    static void DoAutoLoot(int32_t a1);
};

int32_t Script_SetLootPortrait(lua_State* L);
int32_t Script_GetNumLootItems(lua_State* L);
int32_t Script_GetLootSlotInfo(lua_State* L);
int32_t Script_GetLootSlotLink(lua_State* L);
int32_t Script_LootSlot(lua_State* L);
int32_t Script_ConfirmLootSlot(lua_State* L);
int32_t Script_CloseLoot(lua_State* L);
int32_t Script_GiveMasterLoot(lua_State* L);
int32_t Script_GetLootRollItemLink(lua_State* L);
int32_t Script_RollOnLoot(lua_State* L);
int32_t Script_ConfirmLootRoll(lua_State* L);
void LootInfoRegisterScriptFunctions();

#endif
