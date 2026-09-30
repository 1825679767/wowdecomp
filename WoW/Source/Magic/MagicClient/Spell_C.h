#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELL_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Net/Types.h"
#include "_Triage/PendingSpellCastData.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

struct lua_State;
class CDataStore;
class SpellRec;
class SpellVisualRec;

class CMapEntity;
class ItemStats_C;
class OverrideSpellDataRec;
class CGItem_C;
class CGObject_C;
class CGUnit_C;
class CTerrainClickEvent;
class PendingSpellCast : public TSLinkedNode<PendingSpellCast> {
    public:
    /* 0008 */ PendingSpellCastData m_data;
    /* 0138 */ uint8_t m_hidden;
    /* 0139 */ uint8_t unk_0139;
    /* 013A */ uint8_t unk_013A [2];
    /* 013C */ uint32_t m_dwordC;
    /* 0140 */ uint32_t unk_0140;
    /* 0144 */ uint32_t unk_0144;

    PendingSpellCast();
    void OnSpellStartPacket(CDataStore* msg);
    void OnSpellGo(CDataStore* msg);
    bool IsCharmedUnitCast() const;
    bool HasValidTransports() const;
};

struct SPELLHISTORY : public TSLinkedNode<SPELLHISTORY> {
    /* 0008 */ int32_t m_spellId;
    /* 000C */ int32_t m_itemId;
    /* 0010 */ uint32_t m_time;
    /* 0014 */ uint32_t m_recoveryTime;
    /* 0018 */ int32_t m_category;
    /* 001C */ uint32_t m_categoryTime;
    /* 0020 */ uint32_t m_categoryRecoveryTime;
    /* 0024 */ uint8_t m_onHold;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ int32_t m_startRecoveryCategory;
    /* 002C */ uint32_t m_startRecoveryTime;
};

struct SpellCastTargetPoint {
    /* 0000 */ C3Vector m_position;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_transport;
    /* 0018 */ C3Vector m_worldPosition;
    /* 0024 */ uint32_t unk_0024;
};

struct NotifyDestLocSpellCastData {
    /* 0000 */ WOWGUID m_caster;
    /* 0008 */ WOWGUID m_destTarget;
    /* 0010 */ int32_t m_spellId;
    /* 0014 */ C3Vector m_destLocation;
    /* 0020 */ C3Vector m_destPosition;
    /* 002C */ float m_elevation;
    /* 0030 */ float unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint8_t unk_0038;
    /* 0039 */ uint8_t m_castCount;
    /* 003A */ uint8_t unk_003A[2];
};

struct SpellInTransit : public TSLinkedNode<SpellInTransit> {
    /* 0008 */ WOWGUID m_caster;
    /* 0010 */ int32_t m_spellId;
    /* 0014 */ uint32_t m_serverTime;
    /* 0018 */ uint32_t m_arrivalTime;
    /* 001C */ uint32_t unk_001C;
};

class SpellHistory {
    public:
    /* 0000 */ TSList<SPELLHISTORY, TSGetLink<SPELLHISTORY>> m_history;
    /* 000C */ TSList<SPELLHISTORY, TSGetLink<SPELLHISTORY>> m_free;

    int32_t IsOnHold(int32_t spellId, int32_t itemId);
    void AddHistory(int32_t spellId, int32_t itemId, uint32_t time, uint32_t recoveryTime, int32_t category, uint32_t categoryTime, uint32_t categoryRecoveryTime, bool onHold, int32_t startRecoveryCategory, uint32_t startRecoveryTime);
    void ClearOnHold(int32_t spellId, uint32_t time, bool remove);
    void GarbageCollect(uint32_t time);
};

class Spell_C {
    public:
    /* 0000 */ uint8_t unk_0000[8];
    /* 0008 */ uint32_t unk_0008;

    static void SendPendingCast(bool a1);
    static void GetSpellRange(CGUnit_C* caster, int32_t spellId, float* minRange, float* maxRange, CGUnit_C* target);
};

int32_t Packet_Group_30(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_FAILED_OTHER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ITEM_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_31(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_COOLDOWN_CHEAT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PET_TAME_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_DELAYED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_CHANNEL_START(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_MSG_CHANNEL_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAY_SPELL_VISUAL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_PLAY_SPELL_IMPACT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_Group_32(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_GAMEOBJECT_RESET_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_FEIGN_DEATH_RESISTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SPELL_UPDATE_CHAIN_TARGETS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_NOTIFY_DEST_LOC_SPELL_CAST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_SET_PROJECTILE_POSITION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_MEETINGSTONE_IN_PROGRESS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
int32_t Packet_SMSG_UNIT_SPELLCAST_START(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
CMapEntity* Spell_C_GetSpellWorldObject();
void Spell_C_UpdateInTransitList();
bool Spell_C_CanTargetTerrain();
bool Spell_C_CanTargetUnits();
bool Spell_C_IsTargeting();
extern PendingSpellCast* s_targetingSpell;
extern uint32_t s_needTargets;
int32_t Spell_C_IsCursorWorldObjectHousing();
void Spell_C_CursorWorldObjectRotate();
void Spell_C_CancelPlayerSpells();
void Spell_C_SetPlayerClass(int32_t classId);
int32_t Spell_C_IsSpellInTransit(WOWGUID caster, uint32_t spellId, uint32_t castTime);

void Spell_C_CancelSpellByTarget(const WOWGUID& target, int32_t reason);

void Spell_C_CancelSpellByCaster(const WOWGUID& caster);
void Spell_C_CancelCastingSpellByMovement();
void Spell_C_CancelChannelSpell(int32_t spellId);
int32_t Spell_C_GetAutoRepeatingSpell();
void Spell_C_SetCooldownLeft(int32_t spellId, uint32_t itemId, uint32_t category, int32_t a4, int32_t cooldown, bool a6, int32_t a7);
int32_t Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, int32_t* minPoints, int32_t* maxPoints, int32_t a5, int32_t a6, int32_t a7, int32_t a8);
void Spell_C_CancelAutoRepeat(bool a1);
void Spell_C_CancelSpellsWithId(int32_t spellId);
void Spell_C_SetAutoRangedCombatSpell(int32_t spellId);
int32_t Spell_C_GetAutoRangedCombatSpell();
void Spell_C_ClearLastAutoRangedCombatError();
bool Spell_C_HandleTerrainClick(const CTerrainClickEvent& evt);
void Spell_C_HandleSpriteClick(CGObject_C* object, PendingSpellCast* spell);
int32_t Spell_C_HandleItemDrop(CGItem_C* item, CGUnit_C* unit);
void Spell_C_CancelMeleeSpells();
bool Spell_C_IsRestrictedAbility(const SpellRec* spell);
bool Spell_C_HasEffect(const SpellRec* spell, int32_t effect);
int32_t Spell_C_IsOverride(int32_t spellId, const OverrideSpellDataRec* rec);
int32_t Spell_C_GetPowerCost(const SpellRec* spell, const CGUnit_C* unit);
bool Spell_C_HaveSpellPower(CGUnit_C* unit, const SpellRec* spell);
int32_t Spell_C_IsActionOnHold(const int32_t* spellId, int32_t index);
int32_t Spell_C_NeedsCooldownEvent(int32_t itemId);
bool Spell_C_IsUsableAction(CGUnit_C* unit, const SpellRec* spell, const WOWGUID& target, const ItemStats_C* item, bool ignoreControl);
bool Spell_C_GetSpellModifiers(const SpellRec* spell, int32_t modifier, int32_t& flat, int32_t& pct);
int32_t Spell_C_CancelsAuraEffect(const SpellRec* spell, int32_t auraType, int32_t a3);
int32_t Spell_C_CancelsCharm(const CGUnit_C* unit, const SpellRec* spell, int32_t* reason);
int32_t Spell_C_HaveSpellTokens(CGUnit_C* unit, const SpellRec* spell, bool a3, PendingSpellCast* cast);
int32_t Spell_C_HaveEquippedSpellItems(CGUnit_C* unit, const SpellRec* spell, bool a3, PendingSpellCast* cast);
float GetGameTableValue(int32_t table, int32_t index, int32_t a3);
void ItemCheckCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

class QuestTextParser {
    public:
    static int32_t Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId);
};
void Spell_C_RemovePeriodicClientTrigger(WOWGUID guid, int32_t a2);

void Spell_C_ClearCooldowns();
const SpellVisualRec* GetSpellVisual(const SpellRec* spell);
void Spell_C_Destroy();
void Spell_C_Initialize();
PendingSpellCast* CreatePendingSpellCast(bool assignCastCount);
PendingSpellCast* GetPendingSpellCast(uint8_t castCount, int32_t spellId, const WOWGUID& caster);
PendingSpellCast* FindPendingSpellCast(const WOWGUID& castItem, PendingSpellCast* fallback, uint8_t castCount, int32_t spellId, const WOWGUID& caster);
void ReleasePendingSpellCast(PendingSpellCast* cast);
void StartSpellCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t castTime, bool hidden);
void ClearSpellCastState(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell);
void SpellVisualsHandleCastStart(CGUnit_C* unit, PendingSpellCastData* data, const SpellRec* spell);
void HandleSpellFailedOrGo(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t reason);
void FinishPendingSpellCast(PendingSpellCast* cast, const SpellRec* spell, CGUnit_C* unit);
void Spell_C_DestroySpellWorldObject();
void Spell_C_AddProcessedDestLocSpellCast(WOWGUID caster, uint8_t count, uint32_t time);
void Spell_C_AddToInTransitList(WOWGUID caster, const SpellRec* spell, uint32_t serverTime, uint32_t arrivalTime);
bool Spell_C_SpellVisualHasChainEffect(const SpellVisualRec* visual);
void Spell_C_PlayDestLocVisualsWithoutCaster(NotifyDestLocSpellCastData* data);
void Spell_C_AddPendingItemCooldown(int32_t itemId, int32_t spellId, uint32_t time, bool onHold);
void Spell_C_PendingItemCooldownCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void PlayImpactVisual(const SpellRec* spell, const TSStackArray<WOWGUID>& targets, CGUnit_C* unit, const SpellVisualRec* visual);
void PlayImpactVisualOnTarget(const SpellRec* spell, const SpellVisualRec* visual, CGUnit_C* target, CGUnit_C* caster);
void LaunchMissileVisual(const TSStackArray<WOWGUID>& hits, const TSStackArray<WOWGUID>& misses, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, PendingSpellCastData* data, const TSStackArray<uint32_t>& missReasons, const TSStackArray<uint32_t>& reflectResults, uint32_t delay);
void CreateMissilesForTargets(const TSStackArray<WOWGUID>& targets, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, PendingSpellCastData* data, const TSStackArray<uint32_t>* missReasons, const TSStackArray<uint32_t>* reflectResults, uint32_t delay);
void CreateMissilesForTargetPoints(const TSStackArray<SpellCastTargetPoint>& points, CGUnit_C* unit, const SpellRec* spell, const SpellVisualRec* visual, uint32_t ammoDisplayId, uint32_t ammoInventoryType, PendingSpellCastData* data, uint32_t delay);
bool Spell_C_HasCooldownOnEvent(const SpellRec* spell, WOWGUID guid);
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, const SpellRec* spell, int32_t pet);
bool Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier);
void Spell_C_SpellFailed(PendingSpellCast* cast, const SpellRec* spell, int32_t reason, int32_t a4, int32_t a5, bool a6);
int32_t Spell_C_CastSpell(CGUnit_C* caster, int32_t spellId, const CGItem_C* item, WOWGUID target, PendingSpellCast* cast, bool a6);
void UnitCombatLogCastStart(const WOWGUID& caster, const WOWGUID& target, int32_t spellId);
void Spell_C_StopTargeting();
void SpellTableDestroy();
void SpellTableInitialize();

int32_t Script_SpellTargetItem(lua_State* L);
int32_t Script_SpellCanTargetUnit(lua_State* L);
int32_t Script_SpellTargetUnit(lua_State* L);
int32_t Script_SpellStopTargeting(lua_State* L);
int32_t Script_SpellStopCasting(lua_State* L);
int32_t Script_CancelUnitBuff(lua_State* L);
int32_t Script_CancelItemTempEnchantment(lua_State* L);
void SpellRegisterScriptFunctions();

bool SpellIsTrackingAura(const SpellRec* spell);
bool SpellHasAuraEffect(const SpellRec* spell, int32_t aura);
bool SpellIsCallCompanion(const SpellRec* spell);
void SpellSetFeedPetSpell(int32_t spellId);
void Spell_C_ClickSpell(int32_t spellId, const CGItem_C* item, WOWGUID target);
int32_t Spell_C_GetSpellReagentUses(const SpellRec* spell);
int32_t Spell_C_GetSpellReagentUses(int32_t spellId);
bool Spell_C_IsCurrentItem(int32_t itemId);
bool Spell_C_IsCurrentSpell(int32_t spellId);
void Spell_C_GetItemCooldown(const CGItem_C* item, uint32_t* duration, uint32_t* start, uint32_t* enable);
void Spell_C_GetItemCooldown(int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable);
void Spell_C_GetSpellCooldown(int32_t spellId, int32_t petSpell, uint32_t* duration, uint32_t* start, uint32_t* enable);
bool Spell_C_IsActiveSpell(const SpellRec* spell, const CGUnit_C* unit);
uint32_t Spell_C_GetPowerDisplayMod(uint32_t powerType);
const char* GetPowerString(uint32_t powerType);

#endif
