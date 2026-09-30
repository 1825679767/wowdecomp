#ifndef WOW_SOURCE_UI_SPELLBOOKFRAME_H
#define WOW_SOURCE_UI_SPELLBOOKFRAME_H

#include <cstdint>
#include "Engine/Source/Base/RCString.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class CGUnit_C;

struct SpellUsableInfo {
    /* 0000 */ int32_t m_spellId;
    /* 0004 */ uint8_t m_usable;
    /* 0005 */ uint8_t m_noMana;
    /* 0006 */ uint8_t unk_0006[2];
};

struct SpellBookSkillInfo {
    /* 0000 */ int32_t m_skillLine;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ uint32_t m_unk08;
};

struct SPELLRANKNODE : public TSLinkedNode<SPELLRANKNODE> {
    /* 0008 */ int32_t m_spellId;
    /* 000C */ int32_t m_petSpell;
    /* 0010 */ RCString m_subtext;
    /* 001C */ uint32_t m_rank;
};

struct SPELLSBYNAME : public TSHashObject<SPELLSBYNAME, HASHKEY_UTF8I> {
    /* 0018 */ TSList<SPELLRANKNODE> m_spells;
};

class CGPlayer_C;
class SpellRec;

class CGSpellBook {
    public:
    static const char* GetSpellTexture(int32_t slot, int32_t petSpell);
    static int32_t IsSelectedSlot(int32_t slot, int32_t petSpell);
    static int32_t FindSpellByName(const char* name, const char* subtext, int32_t* petSpell);
    static int32_t m_autoRangedSpell;
    static void UpdateCooldowns();
    static int32_t IsTrackingSpell(int32_t spellId);
    static TSFixedArray<int32_t> m_languageSpells;
    static TSGrowableArray<int32_t> m_unlearnedSpells;
    static int32_t m_duelSpell;
    static int32_t m_stuckSpell;
    static uint32_t m_numPetSpells;
    static int32_t m_summonFriendSpell;
    static TSGrowableArray<int32_t> m_companions[2];

    static void AddKnownSpell(int32_t spellId, int32_t notify, int32_t replaceSpell);
    static void AddSpellToNameCache(int32_t spellId, int32_t a2);
    static void CompanionCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static int32_t GetSpellProgressionRank(const SpellRec* spell);
    static int32_t GetCompanionType(const SpellRec* spell);
    static int32_t FindCompanionSlotByID(int32_t spellId, int32_t type);
    static TSGrowableArray<int32_t> m_totemSpells[4];
    static uint32_t m_interruptSchoolMask;
    static uint32_t m_interruptSpecialSchoolMask;
    static uint32_t m_silenceSchoolMask;
    static uint32_t m_silenceSpecialSchoolMask;

    static void ClearSpells();
    static void UpdateCompanions();
    static void UpdateSelection();
    static void UpdateUsablePower();
    static void UpdateSpells(int32_t a1, int32_t a2, int32_t a3);
    static void UpdateUsable();
    static void CastSpell(int32_t spellId, int32_t petSpell, WOWGUID target);
    static void InitializeGame();
    static void MakeKnownSpellModelsLocal();
    static int32_t GetSkinningSpell(const CGUnit_C* unit);
    static void ReplaceSpell(int32_t oldSpell, int32_t newSpell);
};

int32_t IsValidSpell(const char* name, int32_t* petSpell);
int32_t GetSpellSlot(int32_t spellId, int32_t petSpell);
int32_t GetSpellIdBySlot(int32_t slot, int32_t petSpell);
bool HasSpell(int32_t spellId, int32_t petSpell);
extern TSGrowableArray<SpellUsableInfo> s_shapeshiftSpells;
extern uint32_t s_numSkillInfos;
extern uint32_t s_unkBE5D88[1024];
extern int32_t s_spellList[1024];
extern uint32_t s_numSpells;
extern uint32_t s_unkBE8D9C;
extern TSGrowableArray<SpellBookSkillInfo*> s_skillInfos;
void PrefetchSpellVisuals(int32_t spellId);

int32_t Script_GetSpellLink(lua_State* L);
int32_t Script_GetSpellInfo(lua_State* L);
int32_t Script_GetSpellAutocast(lua_State* L);
int32_t Script_ToggleSpellAutocast(lua_State* L);
int32_t Script_EnableSpellAutocast(lua_State* L);
int32_t Script_DisableSpellAutocast(lua_State* L);
int32_t Script_PickupSpell(lua_State* L);
int32_t Script_CastSpell(lua_State* L);
int32_t Script_IsCurrentSpell(lua_State* L);
int32_t Script_IsUsableSpell(lua_State* L);
int32_t Script_SpellHasRange(lua_State* L);
int32_t Script_IsSpellInRange(lua_State* L);
int32_t Script_GetShapeshiftForm(lua_State* L);
int32_t Script_CancelShapeshiftForm(lua_State* L);
int32_t Script_GetShapeshiftFormInfo(lua_State* L);
int32_t Script_CastShapeshiftForm(lua_State* L);
int32_t Script_GetShapeshiftFormCooldown(lua_State* L);
int32_t Script_CastSpellByName(lua_State* L);
int32_t Script_CastSpellByID(lua_State* L);
int32_t Script_GetCompanionCooldown(lua_State* L);
int32_t Script_PickupCompanion(lua_State* L);
int32_t Script_CallCompanion(lua_State* L);
int32_t Script_DismissCompanion(lua_State* L);
int32_t Script_SummonRandomCritter(lua_State* L);
void SpellBookRegisterScriptFunctions();

int32_t GetActionBarPageOffset(const CGPlayer_C* player, const SpellRec* spell);
int __cdecl SortSpellUsableInfo(const void* a, const void* b);

int __cdecl SortSkillInfoByName(const void* a, const void* b);
int __cdecl SortSpellsForDisplay(const void* a, const void* b);
int __cdecl SortCompanions(const void* a, const void* b);

extern TSGrowableArray<int32_t> s_trackingSpells;

extern int32_t s_spellList[1024];
extern uint32_t s_numSpells;
extern uint32_t s_petSpellList[1024];
extern TSBitField<TSFixedArray<uint32_t> > s_knownSpellBits;

int32_t FindSpellSlotByName(const char* name, const char* subtext, int32_t* petSpell);
int32_t GetSpellIdByName(const char* name, int32_t* petSpell);
bool GetSlotFromLua(lua_State* L, uint32_t* slot, int32_t* petSpell, int32_t* type, int32_t allowName);
bool HasPetSpell(int32_t spellId);
extern TSGrowableArray<int32_t> s_openLockSpells;

#endif
