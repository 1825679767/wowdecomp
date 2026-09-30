#include "Spell_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "WoW/Source/DB/Rec/SpellMissileRec.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/DB/Rec/SpellCategoryRec.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include <cmath>
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/WowServices/WDataStore.h"
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif

// STATIC: 0x00D3F5AC
static SpellHistory s_spellHistory[2];

// STATIC: 0x00D397C8
static int32_t s_lastAutoRangedCombatError;

// STATIC: 0x00D397CC
static int32_t s_autoRangedCombatSpell;

// STATIC: 0x00D397D0
static int32_t s_autoRepeatingSpell;

// STATIC: 0x00D3F4E0
uint32_t s_needTargets;

// STATIC: 0x00D3F4E4
PendingSpellCast* s_targetingSpell;

// STATIC: 0x00D3F4E8
static uint32_t unk_D3F4E8;

// STATIC: 0x00D397B0
static CVar* s_enableErrorSpeechCVar;

// STATIC: 0x00D397B4
static int32_t s_spellClassSet;

// STATIC: 0x00DCE234
static const CSoftFloat s_softPercent = { 0x3C23D70A };

// STATIC: 0x00D397B8
static uint32_t unk_D397B8;

// STATIC: 0x00D397C4
static uint32_t unk_D397C4;

// STATIC: 0x00D397D8
static int32_t s_spellPctModifiers[96][31];

// STATIC: 0x00D3C658
static int32_t s_spellFlatModifiers[96][31];

// STATIC: 0x00AF524C
static TSList<PendingSpellCast, TSGetLink<PendingSpellCast> > s_pendingSpellCasts;

// STATIC: 0x00AF5258
static TSList<PendingSpellCast, TSGetLink<PendingSpellCast> > s_freePendingSpellCasts;

// STATIC: 0x00D397D5
static uint8_t s_nextCastCount;

// STATIC: 0x00D3F570
static CMapEntity* s_spellWorldObject;

// STATIC: 0x00D3F574
static CM2Model* s_spellWorldModel;

// STATIC: 0x00AF5270
static TSList<SpellInTransit, TSGetLink<SpellInTransit>> s_spellsInTransit;

// STATIC: 0x00D3F608
static uint32_t s_nextHistoryCleanup;

// STATIC: 0x00AF5220
static uint32_t s_powerDisplayMod[7] = { 1, 10, 1, 1, 1000, 1, 10 };

// STATIC: 0x00D397D4
static uint8_t s_cursorWorldObjectHousing;

// STATIC: 0x00D3F4DC
static float s_cursorWorldObjectFacing;

// VERIFY: f852b51c manual
// OFFSET: 0x008008D0
void Spell_C_CancelChannelSpell(int32_t spellId) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CANCEL_CHANNELLING));
    msg.Put(static_cast<uint32_t>(spellId));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: fe82041e manual
// OFFSET: 0x007FD620
bool Spell_C_IsTargeting() {
    return s_targetingSpell != nullptr;
}

// VERIFY: e3716b07 auto
// OFFSET: 0x007FE130
int32_t Spell_C_GetAutoRepeatingSpell() {
    return s_autoRepeatingSpell;
}

// VERIFY: e95a2641 auto
// OFFSET: 0x007FE140
void Spell_C_SetAutoRangedCombatSpell(int32_t spellId) {
    if (s_autoRangedCombatSpell != spellId) {
        s_autoRangedCombatSpell = spellId;

        if (!s_autoRepeatingSpell) {
            if (spellId) {
                FrameScript_SignalEvent(SCRIPT_EVENT_START_AUTOREPEAT_SPELL, nullptr);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_STOP_AUTOREPEAT_SPELL, nullptr);
            }
        }
    }
}

// VERIFY: 1930d71d auto
// OFFSET: 0x007FE180
int32_t Spell_C_GetAutoRangedCombatSpell() {
    return s_autoRangedCombatSpell;
}

// VERIFY: 94e1f1cb auto
// OFFSET: 0x007FE190
void Spell_C_ClearLastAutoRangedCombatError() {
    s_lastAutoRangedCombatError = 187;
}

// VERIFY: e5501e2c manual
// OFFSET: 0x0080C340
bool Spell_C_HandleTerrainClick(const CTerrainClickEvent& evt) {
    PendingSpellCast* spell = s_targetingSpell;

    if (!spell) {
        return false;
    }

    uint32_t needTargets = s_needTargets;

    if (needTargets & 0x20) {
        spell->m_data.m_sourceTarget = evt.m_guid;
        spell->m_data.m_sourcePos = evt.m_point;
        spell->m_data.m_targetFlags |= 0x20;
        needTargets &= ~0x20u;
    } else if (needTargets & 0x40) {
        spell->m_data.m_destTarget = evt.m_guid;
        spell->m_data.m_destPos = evt.m_point;
        spell->m_data.m_targetFlags |= 0x40;
        needTargets &= ~0x40u;
    } else {
        return false;
    }

    s_needTargets = needTargets;

    if (!needTargets) {
        Spell_C::SendPendingCast(false);
    }

    return true;
}

// VERIFY: bfa71bbe auto
// OFFSET: 0x007FCC70
bool Spell_C_IsRestrictedAbility(const SpellRec* spell) {
    if (!spell) {
        return false;
    }

    return (spell->m_attributes & 0x404) || (spell->m_attributesEx & 0x200) || (spell->m_attributesExB & 0x100000);
}

// VERIFY: e2735a7d auto
// OFFSET: 0x007FDE20
bool Spell_C_HasEffect(const SpellRec* spell, int32_t effect) {
    for (uint32_t i = 0; i < sizeof(spell->m_effect) / sizeof(spell->m_effect[0]); i++) {
        if (spell->m_effect[i] == effect) {
            return true;
        }
    }

    return false;
}

// VERIFY: 4b3db043 auto
// OFFSET: 0x007FE820
int32_t Spell_C_IsOverride(int32_t spellId, const OverrideSpellDataRec* rec) {
    for (uint32_t i = 0; i < sizeof(rec->m_spells) / sizeof(rec->m_spells[0]); i++) {
        if (rec->m_spells[i] == spellId) {
            return 1;
        }
    }

    return 0;
}

// VERIFY: 47619e72 manual
// OFFSET: 0x008012F0
int32_t Spell_C_GetPowerCost(const SpellRec* spell, const CGUnit_C* unit) {
    if (!unit) {
        return -1;
    }

    uint32_t rank = unit->GetSpellRank(spell);
    int32_t cost = spell->m_manaCost + spell->m_manaCostPerLevel * (static_cast<int32_t>(rank / 5) - spell->m_baseLevel);

    if (spell->m_manaCostPct) {
        cost += spell->m_manaCostPct * static_cast<int32_t>(unit->GetBasePowerForType(spell->m_powerType)) / 100;
    }

    if ((spell->m_attributesExD & 0x400) && const_cast<CGUnit_C*>(unit)->IsActivePlayer()) {
        uint32_t speed = 0;
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(unit->GetShapeshiftForm());

        if (form && form->m_combatRoundTime) {
            speed = form->m_combatRoundTime;
        } else {
            const CGItem_C* item = const_cast<CGUnit_C*>(unit)->GetBag()->GetItemPointer(((spell->m_attributesExC & 0x1000000) != 0) + 15);

            if (item && item->IsUsable() && !unit->IsDisarmed((spell->m_attributesExC >> 24) & 0x1)) {
                WOWGUID none;
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, nullptr, nullptr, false);

                if (stats) {
                    speed = stats->m_delay;
                }
            } else {
                speed = 2000;
            }
        }

        cost += speed / 100;
    }

    int32_t school = spell->m_schoolMask;
    float modified = static_cast<float>(cost + unit->GetSpellCostModifier(school));
    int32_t base = CMath::fint_(unit->GetSpellCostMultiplier(school) * modified);
    cost = base;

    int32_t flat;
    int32_t pct;

    if (Spell_C_GetSpellModifiers(spell, 14, flat, pct)) {
        cost = (flat + base) * pct / 100;
    }

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER) && (spell->m_attributes & 0x80000) && spell->m_spellLevel) {
        float spellLevelValue = GetGameTableValue(6, spell->m_spellLevel - 1, 0);
        float unitLevelValue = GetGameTableValue(6, unit->m_unit->m_unitFieldLevel - 1, 0);

        if (spellLevelValue > 0.0f && unitLevelValue > 0.0f) {
            cost = CMath::fint_(unitLevelValue / spellLevelValue * cost);
        }
    }

    if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && Spell_C_HasEffect(spell, 97)) {
        for (uint32_t i = 0; i < sizeof(spell->m_effect) / sizeof(spell->m_effect[0]); i++) {
            if (spell->m_effect[i] != 97) {
                continue;
            }

            uint32_t first = spell->m_effectMiscValue[i];
            int32_t count = spell->m_effectMiscValueB[i];

            if (first >= 12 || first + count > 12) {
                continue;
            }

            for (int32_t j = 0; j < count; j++) {
                int32_t petSpell;
                int32_t spellId = CGActionBar::GetSpell(first + j + 132, petSpell);

                if (!spellId || (spellId & 0xF0000000)) {
                    continue;
                }

                SpellRec totemSpell;

                if (g_spellDB.GetRecord(spellId, &totemSpell) && (totemSpell.m_attributesExG & 0x20)) {
                    cost += Spell_C_GetPowerCost(&totemSpell, unit);
                }
            }
        }
    }

    return cost <= 0 ? 0 : cost;
}

// VERIFY: fc34e07a manual
// OFFSET: 0x008017E0
bool Spell_C_HaveSpellPower(CGUnit_C* unit, const SpellRec* spell) {
    if (!unit) {
        return false;
    }

    int32_t powerType = spell->m_powerType;

    if (powerType == -2) {
        int32_t cost = Spell_C_GetPowerCost(spell, unit);
        return cost < static_cast<int32_t>(unit->GetPredictedHealth());
    }

    if (powerType == 5) {
        return unit->IsActivePlayer() && CGRuneInfo::HasRunesForSpell(spell->m_id, nullptr);
    }

    int32_t power = unit->GetPredictedPower(powerType);
    return Spell_C_GetPowerCost(spell, unit) <= power;
}

// VERIFY: 7772af94 manual
// OFFSET: 0x00802850
int32_t QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(int32_t spellId) {
    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell) || !spell.m_difficulty) {
        return spellId;
    }

    SpellDifficultyRec* difficulty = g_spellDifficultyDB.GetRecord(spell.m_difficulty);

    if (!difficulty) {
        return spellId;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        return spellId;
    }

    uint32_t index;

    if (map->m_instanceType == 2) {
        index = CGGameUI::GetCurrentRaidDifficulty();
    } else if (map->m_instanceType == 1) {
        index = CGGameUI::GetCurrentDungeonDifficulty();
    } else {
        return spellId;
    }

    int32_t result = difficulty->m_difficultySpellID[index];

    if (result || (index == 3 && (result = difficulty->m_difficultySpellID[1]) != 0)) {
        return result;
    }

    result = difficulty->m_difficultySpellID[0];
    return result ? result : spellId;
}

// VERIFY: 36d7f927 auto
// OFFSET: 0x008091B0
int32_t Spell_C_IsActionOnHold(const int32_t* spellId, int32_t index) {
    return s_spellHistory[index].IsOnHold(*spellId, 0);
}

// VERIFY: 668afa49 manual
// OFFSET: 0x0080AC10
int32_t Spell_C_NeedsCooldownEvent(int32_t itemId) {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, player, ItemCheckCooldownCallback, nullptr, true);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < sizeof(stats->m_spellId) / sizeof(stats->m_spellId[0]); i++) {
        if (static_cast<int32_t>(stats->m_spellId[i]) > 0 && !stats->m_spellTrigger[i]) {
            return s_spellHistory[0].IsOnHold(stats->m_spellId[i], itemId);
        }
    }

    return 0;
}

// VERIFY: 091fab9f manual
// OFFSET: 0x0080B5D0
bool Spell_C_IsUsableAction(CGUnit_C* unit, const SpellRec* spell, const WOWGUID& target, const ItemStats_C* item, bool ignoreControl) {
    if (!unit) {
        return false;
    }

    if (spell->m_effect[0] == 47) {
        return true;
    }

    if (unit->IsActivePlayer()) {
        bool attack = spell->m_effect[0] == 78 || (spell->m_attributes & 0x2);
        uint32_t flags = static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags;

        if (flags & 0x1000000) {
            if (attack || Spell_C_IsRestrictedAbility(spell)) {
                return false;
            }
        } else if (!attack && (flags & 0x800000)) {
            return false;
        }
    }

    if (unit->IsDeadOrGhost() && !(spell->m_attributes & 0x800000)) {
        return false;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map) {
        if (map->m_instanceType == 4) {
            if (!(spell->m_attributesExD & 0x20000) && (spell->m_recoveryTime > 900000 || spell->m_categoryRecoveryTime > 900000 || (spell->m_attributesExD & 0x10000))) {
                return false;
            }
        } else if (spell->m_attributesExF & 0x2) {
            return false;
        }
    }

    if (!CGGameUI::m_hasControl && !ignoreControl) {
        int32_t allowed = 0;

        if ((unit->m_unit->m_unitFieldFlags & 0x800000) && ((spell->m_attributesExE & 0x20000) || Spell_C_CancelsAuraEffect(spell, 7, 0))) {
            allowed = 1;
        }

        if ((unit->m_unit->m_unitFieldFlags & 0x400000) && ((spell->m_attributesExE & 0x40000) || Spell_C_CancelsAuraEffect(spell, 5, 0))) {
            allowed = 1;
        }

        if (unit->IsCharmed() && Spell_C_CancelsCharm(unit, spell, nullptr)) {
            allowed = 1;
        }

        if ((!(unit->m_unit->m_unitFieldFlags & 0x100000) || !(spell->m_attributes & 0x1000000)) && !allowed) {
            return false;
        }
    }

    if ((!item || !(item->m_flags[0] & 0x10000000)) && !Spell_C_HaveSpellTokens(unit, spell, false, nullptr)) {
        return false;
    }

    if (!Spell_C_HaveEquippedSpellItems(unit, spell, true, nullptr)) {
        return false;
    }

    if (spell->m_attributesEx & 0x500000) {
        if (unit->IsActivePlayer() && !CGGameUI::GetComboPoints(target) && !unit->IgnoresAuraState(spell)) {
            return false;
        }

        if (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && !CGPetInfo::GetComboPoints(target)) {
            return false;
        }
    }

    int32_t reason;

    if (!unit->CheckShapeshiftRules(spell, &reason) && (reason == 94 || !unit->CanAutoCancelShapeshift())) {
        return false;
    }

    if (!unit->CheckStealthRules(spell, &reason) || ((spell->m_attributes & 0x10000000) && (unit->m_unit->m_unitFieldFlags & 0x80000) && !unit->IgnoresAuraState(spell))) {
        return false;
    }

    if (spell->m_casterAuraState && !unit->HasAuraState(spell->m_casterAuraState) && !unit->IgnoresAuraState(spell)) {
        return false;
    }

    if (spell->m_excludeCasterAuraState && unit->HasAuraState(spell->m_excludeCasterAuraState) && !unit->IgnoresAuraState(spell)) {
        return false;
    }

    int32_t casterAura = QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(spell->m_casterAuraSpell);

    if (casterAura && !unit->HasAura(casterAura) && !unit->IgnoresAuraState(spell)) {
        return false;
    }

    int32_t excludeCasterAura = QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(spell->m_excludeCasterAuraSpell);

    if (excludeCasterAura && unit->HasAura(excludeCasterAura) && !unit->IgnoresAuraState(spell)) {
        return false;
    }

    int32_t targetAura = QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(spell->m_targetAuraSpell);
    int32_t excludeTargetAura = QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(spell->m_excludeTargetAuraSpell);

    if (spell->m_targetAuraState || spell->m_excludeTargetAuraState || targetAura || excludeTargetAura) {
        WOWGUID guid = target;

        if (!guid) {
            guid = CGGameUI::m_lockedTarget;
        }

        CGUnit_C* targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (g_autoSelfCastCVar->m_intValue) {
            int32_t implicit = spell->m_implicitTargetA[0];

            if (!targetUnit || ((implicit == 21 || implicit == 35 || implicit == 57 || implicit == 61) && !unit->CanAssist(targetUnit, false))) {
                targetUnit = ClntObjMgrGetActivePlayerObj();
            }
        }

        if (targetUnit) {
            if (spell->m_targetAuraState && !targetUnit->HasAuraState(spell->m_targetAuraState) && !unit->IgnoresAuraState(spell)) {
                return false;
            }

            if (spell->m_excludeTargetAuraState && targetUnit->HasAuraState(spell->m_excludeTargetAuraState) && !unit->IgnoresAuraState(spell)) {
                return false;
            }

            if (targetAura && !targetUnit->HasAura(targetAura) && !unit->IgnoresAuraState(spell)) {
                return false;
            }

            if (excludeTargetAura && targetUnit->HasAura(excludeTargetAura) && !unit->IgnoresAuraState(spell)) {
                return false;
            }

            if (spell->m_implicitTargetA[0] == 6 && !unit->CanAttack(targetUnit)) {
                return false;
            }

            if (spell->m_implicitTargetA[0] == 21 && !unit->CanAssist(targetUnit, false)) {
                return false;
            }

            if (spell->m_implicitTargetA[0] == 35 && (!unit->IsInMyPartyOrRaid(targetUnit) || !unit->CanAssist(targetUnit, false))) {
                return false;
            }

            int32_t implicit = spell->m_implicitTargetA[0];

            if ((implicit == 57 || implicit == 61) && (!unit->IsInMyParty(targetUnit) || !unit->CanAssist(targetUnit, false))) {
                return false;
            }
        } else if (spell->m_targetAuraState || targetAura) {
            return false;
        }
    }

    if ((spell->m_attributes & 0x2000000) && Spell_C_IsActionOnHold(&spell->m_id, 0)) {
        return false;
    }

    if (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && !CGPetInfo::PetActionsUsable(false)) {
        return false;
    }

    int32_t interruptFlags = spell->m_auraInterruptFlags;

    if (((interruptFlags & 0x100) || ((spell->m_attributesEx & 0x44) && (spell->m_channelInterruptFlags & 0x100))) && !(unit->m_clientFlags & 0x200000)) {
        return false;
    }

    if (((interruptFlags & 0x80) || ((spell->m_attributesEx & 0x44) && (spell->m_channelInterruptFlags & 0x80))) && (unit->m_clientFlags & 0x200000)) {
        return false;
    }

    if (spell->m_attributes & 0xC000) {
        if (unit->IsOutdoors()) {
            if (spell->m_attributes & 0x4000) {
                return false;
            }
        } else if (spell->m_attributes & 0x8000) {
            return false;
        }
    }

    return (!(spell->m_attributesExD & 0x4000000) || static_cast<CGPlayer_C*>(unit)->IsInFlyableZone(1)) && (!(spell->m_attributesExC & 0x800) || !map || map->m_instanceType == 3) && (!(spell->m_attributesExF & 0x800) || !map || map->m_instanceType != 2);
}

// VERIFY: e4a633f9 auto
// OFFSET: 0x008100E0
void Spell_C_Initialize() {
    ClientServices::SetMessageHandler(SMSG_SPELL_START, Packet_Group_30, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_GO, Packet_Group_30, nullptr);
    ClientServices::SetMessageHandler(SMSG_CAST_FAILED, Packet_SMSG_CAST_FAILED, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_FAILURE, Packet_SMSG_SPELL_FAILURE, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_FAILED_OTHER, Packet_SMSG_SPELL_FAILED_OTHER, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_CAST_FAILED, Packet_SMSG_PET_CAST_FAILED, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_COOLDOWN, Packet_SMSG_SPELL_COOLDOWN, nullptr);
    ClientServices::SetMessageHandler(SMSG_ITEM_COOLDOWN, Packet_SMSG_ITEM_COOLDOWN, nullptr);
    ClientServices::SetMessageHandler(SMSG_COOLDOWN_EVENT, Packet_Group_31, nullptr);
    ClientServices::SetMessageHandler(SMSG_CLEAR_COOLDOWN, Packet_Group_31, nullptr);
    ClientServices::SetMessageHandler(SMSG_MODIFY_COOLDOWN, Packet_Group_31, nullptr);
    ClientServices::SetMessageHandler(SMSG_COOLDOWN_CHEAT, Packet_SMSG_COOLDOWN_CHEAT, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_TAME_FAILURE, Packet_SMSG_PET_TAME_FAILURE, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_DELAYED, Packet_SMSG_SPELL_DELAYED, nullptr);
    ClientServices::SetMessageHandler(MSG_CHANNEL_START, Packet_MSG_CHANNEL_START, nullptr);
    ClientServices::SetMessageHandler(MSG_CHANNEL_UPDATE, Packet_MSG_CHANNEL_UPDATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_SPELL_VISUAL, Packet_SMSG_PLAY_SPELL_VISUAL, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_SPELL_IMPACT, Packet_SMSG_PLAY_SPELL_IMPACT, nullptr);
    ClientServices::SetMessageHandler(SMSG_SET_FLAT_SPELL_MODIFIER, Packet_Group_32, nullptr);
    ClientServices::SetMessageHandler(SMSG_SET_PCT_SPELL_MODIFIER, Packet_Group_32, nullptr);
    ClientServices::SetMessageHandler(SMSG_GAME_OBJECT_RESET_STATE, Packet_SMSG_GAMEOBJECT_RESET_STATE, nullptr);
    ClientServices::SetMessageHandler(SMSG_FEIGN_DEATH_RESISTED, Packet_SMSG_FEIGN_DEATH_RESISTED, nullptr);
    ClientServices::SetMessageHandler(SMSG_SPELL_UPDATE_CHAIN_TARGETS, Packet_SMSG_SPELL_UPDATE_CHAIN_TARGETS, nullptr);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DEST_LOC_SPELL_CAST, Packet_SMSG_NOTIFY_DEST_LOC_SPELL_CAST, nullptr);
    ClientServices::SetMessageHandler(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, nullptr);
    ClientServices::SetMessageHandler(SMSG_SET_PROJECTILE_POSITION, Packet_SMSG_SET_PROJECTILE_POSITION, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESET_RANGED_COMBAT_TIMER, Packet_SMSG_MEETINGSTONE_IN_PROGRESS, nullptr);
    ClientServices::SetMessageHandler(SMSG_RESUME_CAST_BAR, Packet_SMSG_UNIT_SPELLCAST_START, nullptr);
    s_needTargets = 0;
    s_targetingSpell = nullptr;
    memset(s_spellFlatModifiers, 0, sizeof(s_spellFlatModifiers));
    memset(s_spellPctModifiers, 0, sizeof(s_spellPctModifiers));
    s_autoRepeatingSpell = 0;
    s_autoRangedCombatSpell = 0;
    s_lastAutoRangedCombatError = 187;
    unk_D397C4 = 0;
    unk_D397B8 = 0;
    s_spellClassSet = 0;
    unk_D3F4E8 = 0;
    s_enableErrorSpeechCVar = CVar::Lookup("Sound_EnableErrorSpeech");
}

// INLINED
inline PendingSpellCast::PendingSpellCast() : unk_0139(0) {
}

// VERIFY: 85ca0dc3 auto
// OFFSET: 0x009AB770
PendingSpellCastData::PendingSpellCastData() {
    this->m_castItem = WOWGUID{ 0 };
    this->unk_0010 = 0;
    this->unk_0014 = 0;
    this->m_spellId = 0;
    this->m_targetFlags = 0;
    this->m_objectTarget = WOWGUID{ 0 };
    this->m_itemTarget = WOWGUID{ 0 };
    this->unk_0038 = 0;
    this->unk_003C = 0;
    this->m_sourceTarget = WOWGUID{ 0 };
    this->m_destTarget = WOWGUID{ 0 };
    this->m_sourcePos = C3Vector();
    this->m_destPos = C3Vector();
    this->unk_006C = -1;
    this->unk_0068 = 0.0f;
    this->unk_0070 = 0;
    this->m_elevation = 0.0f;
    this->unk_0074 = 0;
    this->unk_0120 = 0.0f;
    this->unk_0078 = 0;
    this->unk_007C = 0;
    this->unk_0080 = 0;
    this->unk_0084 = 0;
    this->unk_0088 = 0;
    this->unk_008C = 0;
    this->unk_0110 = 0;
    this->m_castFlags = 0;
    this->unk_0118 = 0;
    this->unk_0124 = 0;
}

// VERIFY: 33b96970 manual
// OFFSET: 0x009AB810
C3Vector PendingSpellCastData::GetDestLocation() const {
    if (!this->m_destTarget.m_guid) {
        return this->m_destPos;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(this->m_destTarget, &matrix, WOWGUID{ 0 }, __FILE__, __LINE__);
    return this->m_destPos * matrix;
}

// VERIFY: 8b44a52e manual
// OFFSET: 0x009AB9C0
void SpellGetCastTargets(PendingSpellCastData* data, CDataStore* msg) {
    msg->Get(data->m_targetFlags);
    SmartGUID guid;

    if (data->m_targetFlags & 0x18A02) {
        *msg >> guid;
        data->m_objectTarget = guid;
    }

    if (data->m_targetFlags & 0x1010) {
        *msg >> guid;
        data->m_itemTarget = guid;
    }

    if (data->m_targetFlags & 0x20) {
        *msg >> guid;
        data->m_sourceTarget = guid;
        msg->Get(data->m_sourcePos.x);
        msg->Get(data->m_sourcePos.y);
        msg->Get(data->m_sourcePos.z);
    }

    if (data->m_targetFlags & 0x40) {
        *msg >> guid;
        data->m_destTarget = guid;
        msg->Get(data->m_destPos.x);
        msg->Get(data->m_destPos.y);
        msg->Get(data->m_destPos.z);
    }

    if (data->m_targetFlags & 0x2000) {
        msg->GetArray(reinterpret_cast<uint8_t*>(data->m_targetString), sizeof(data->m_targetString));
    }
}

// VERIFY: 672a78e9 manual
// OFFSET: 0x00805010
PendingSpellCast* CreatePendingSpellCast(bool assignCastCount) {
    PendingSpellCast* cast = s_freePendingSpellCasts.Head();

    if (!cast) {
        cast = s_freePendingSpellCasts.NewNode(2, 0, 8);
    }

    s_pendingSpellCasts.LinkToHead(cast);
    memset(&cast->m_data, 0, sizeof(PendingSpellCastData));
    cast->m_hidden = 0;
    cast->unk_0139 = 0;

    if (assignCastCount) {
        uint8_t castCount = s_nextCastCount;

        if (!castCount) {
            castCount = 1;
        }

        cast->m_data.m_castCount = castCount;
        s_nextCastCount = castCount + 1;
    }

    return cast;
}

// VERIFY: e1f3a7c3 manual
// OFFSET: 0x00805100
void ReleasePendingSpellCast(PendingSpellCast* cast) {
    for (PendingSpellCast* node = s_pendingSpellCasts.Head(); node; node = s_pendingSpellCasts.Next(node)) {
        if (node == cast) {
            s_freePendingSpellCasts.LinkToTail(cast);
            return;
        }
    }
}

// VERIFY: 4917a9ff manual
// OFFSET: 0x00805180
PendingSpellCast* GetPendingSpellCast(uint8_t castCount, int32_t spellId, const WOWGUID& caster) {
    if (!spellId) {
        return nullptr;
    }

    for (PendingSpellCast* cast = s_pendingSpellCasts.Head(); cast; cast = s_pendingSpellCasts.Next(cast)) {
        if (cast->m_data.m_castCount == castCount && cast->m_data.m_spellId == spellId && cast->m_data.m_caster == caster) {
            return cast;
        }
    }

    return nullptr;
}

// VERIFY: 01cc0239 auto
// OFFSET: 0x008051D0
PendingSpellCast* FindPendingSpellCast(const WOWGUID& castItem, PendingSpellCast* fallback, uint8_t castCount, int32_t spellId, const WOWGUID& caster) {
    PendingSpellCast* cast = GetPendingSpellCast(castCount, spellId, caster);

    if (cast) {
        return cast;
    }

    if (caster.m_guid && caster == ClntObjMgrGetActivePlayer()) {
        cast = CreatePendingSpellCast(false);
    } else {
        cast = fallback;
    }

    cast->m_data.m_castCount = castCount;
    cast->m_data.m_spellId = spellId;
    cast->m_data.m_castItem = castItem;
    cast->m_data.m_caster = caster;
    return cast;
}

// VERIFY: 1cc8cccc manual
// OFFSET: 0x007FE8D0
bool PendingSpellCast::IsCharmedUnitCast() const {
    if (!ClntObjMgrGetActivePlayerObj()) {
        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    const WOWGUID& controlled = player->m_unit->m_unitFieldCharm.m_guid ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
    return controlled == this->m_data.m_caster;
}

// VERIFY: 160dd50b auto
// OFFSET: 0x007FEC00
void ClearSpellCastState(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell) {
    unit->ClearCastingSpell(unit->m_castSpellId, unit->m_unit->m_unitChannelSpell == 0, true);
    unit->ClearEffectKit(cast->m_data.m_spellId, 4, 0, 0, 0);
    const SpellVisualRec* visual = GetSpellVisual(spell);

    if (visual) {
        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit);

        if (kit && (kit->m_startAnimID || kit->m_animID)) {
            unit->UpdateBaseAnimation(0, -1);
        }
    }

    if (unit->unk_09CC == static_cast<uint32_t>(cast->m_data.m_spellId)) {
        if (unit->m_mountDisplayId != static_cast<int32_t>(unit->m_unit->m_unitFieldMountdisplayid)) {
            unit->OnMountDisplayChanged(unit->m_unit->m_unitFieldMountdisplayid);
        }

        unit->unk_09CC = 0;
    }

    unit->StopUpdatingMissileTrajectory(cast->m_data.m_spellId);
}

// VERIFY: 29db1480 manual
// OFFSET: 0x007FA2E0
void SpellVisualsHandleCastStart(CGUnit_C* unit, PendingSpellCastData* data, const SpellRec* spell) {
    if (!unit) {
        SErrSetLastError(87);
        return;
    }

    if (spell->m_attributes & 0x2) {
        unit->SetSheatheState(2, 1, 0);
    }

    if (spell->m_attributesExB & 0x20) {
        if (unit->m_animationState & 0x200) {
            return;
        }

        unit->SetCastingAutoRepeat(true);
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual = unit->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

    if (visual) {
        if (visual->m_hasMissile && visual->m_missileModel && (visual->m_missileModel == -1 || visual->m_missileModel == -2)) {
            unit->SetSheatheState(1, 1, 0);
        }

        SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit);

        if (kit) {
            if (kit->m_startAnimID > 0 || kit->m_animID > 0) {
                unit->SetCastingAnim(true);
            }

            PlaySpellVisuaKitData kitData(const_cast<SpellRec*>(spell), kit, 4);
            unit->PlaySpellVisualKit(kitData);
        }

        if (unit->IsActivePlayer() && (spell->m_attributes & 0x400000) && (data->m_targetFlags & 0x2) && data->m_objectTarget.m_guid) {
            if (!(spell->m_facingCasterFlags & 0x1) || unit->IsInFrontOfTargetWithGuid(data->m_objectTarget)) {
                unit->SetTrackingTarget(data->m_objectTarget, 0);
            }
        }

        if (visual->m_flags & 0x20) {
            unit->ApplySpellVisualTrail(visual, data->m_castCount);
        }
    }
}

// VERIFY: 98e97d79 manual
// OFFSET: 0x00805330
void StartSpellCastVisuals(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t castTime, bool hidden) {
    if (!unit) {
        return;
    }

    cast->m_hidden = hidden;
    PendingSpellCast* current = GetPendingSpellCast(unit->m_castCount, unit->m_castSpellId, unit->m_obj->m_guid);

    if (current && current->m_hidden) {
        if (!hidden) {
            ClearSpellCastState(unit, current, spell);

            if (unit == ClntObjMgrGetActivePlayerObj()) {
                CGTradeSkillInfo::CommitPendingRecast(cast->m_data.m_spellId);
            }
        }
    } else if (!hidden && unit == ClntObjMgrGetActivePlayerObj()) {
        CGTradeSkillInfo::CommitPendingRecast(cast->m_data.m_spellId);
    }

    if (!unit->m_castSpellId && !cast->unk_0139) {
        unit->SetCastingSpell(cast->m_data.m_castCount, cast->m_data.m_spellId);
        SpellVisualsHandleCastStart(unit, &cast->m_data, spell);
    }

    if (castTime > 0) {
        bool showCastBar = true;

        if (!(spell->m_attributesExD & 0x8000000)) {
            if (spell->m_attributes & 0x2) {
                showCastBar = false;
            }

            if (cast->m_data.m_caster != ClntObjMgrGetActivePlayer() && ((spell->m_attributes & 0x180) || (spell->m_attributesExD & 0x1))) {
                showCastBar = false;
            }
        }

        if (showCastBar) {
            if (cast->m_data.m_spellId != unit->m_castSpellId) {
                ClearSpellCastState(unit, cast, spell);
                unit->SetCastingSpell(cast->m_data.m_castCount, cast->m_data.m_spellId);
                SpellVisualsHandleCastStart(unit, &cast->m_data, spell);
            }

            unit->SetUICastingSpell(cast->m_data.m_spellId, cast->m_data.m_objectTarget, castTime);
            Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 324);
        }
    }

    if ((cast->m_data.m_caster == ClntObjMgrGetActivePlayer() || cast->IsCharmedUnitCast()) && cast->m_data.m_castCount) {
        CGSpellBook::UpdateSelection();
    }
}

// VERIFY: 0e9e554a manual
// OFFSET: 0x00806700
void PendingSpellCast::OnSpellStartPacket(CDataStore* msg) {
    WOWGUID caster = this->m_data.m_caster;
    uint32_t castFlags = this->m_data.m_castFlags;
    uint32_t castTime;
    msg->Get(castTime);
    SpellGetCastTargets(&this->m_data, msg);
    uint32_t power = 0;
    uint32_t hasPower = castFlags & 0x800;

    if (hasPower) {
        msg->Get(power);
    }

    uint8_t runeCooldowns[6] = { 0, 0, 0, 0, 0, 0 };
    uint8_t runesBefore = 0;
    uint8_t runesAfter = 0;
    uint32_t hasRunes = castFlags & 0x200000;

    if (hasRunes) {
        msg->Get(runesBefore);
        msg->Get(runesAfter);

        for (uint32_t i = 0; i < 6; i++) {
            if (((1 << i) & runesBefore) && !((1 << i) & runesAfter)) {
                msg->Get(runeCooldowns[i]);
            }
        }
    }

    uint32_t ammoDisplayId = 0;
    uint32_t ammoInventoryType = 0;

    if (castFlags & 0x20) {
        msg->Get(ammoDisplayId);
        msg->Get(ammoInventoryType);
    }

    uint32_t schoolImmunities = 0;
    uint32_t immunities = 0;
    uint32_t hasImmunities = castFlags & 0x4000000;

    if (hasImmunities) {
        msg->Get(schoolImmunities);
        msg->Get(immunities);
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(this->m_data.m_spellId, &spell)) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (hasPower && !(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
        if (spell.m_powerType == -2) {
            unit->SetPredictedHealth(power);
        } else if (spell.m_powerType == 5) {
            if (caster == ClntObjMgrGetActivePlayer() && hasRunes) {
                CGRuneInfo::SetPredictedRunes(runesBefore, runesAfter, runeCooldowns);
            }
        } else {
            unit->SetPredictedPower(spell.m_powerType, power);
        }

        if (spell.m_powerType == -2) {
            unit->SetPredictedHealth(power);
        } else {
            unit->SetPredictedPower(spell.m_powerType, power);
        }
    }

    if (hasImmunities) {
        unit->SetCastSchoolImmunities(schoolImmunities);
        unit->SetCastImmunities(immunities);
    }

    uint32_t hidden = castFlags & 0x1;

    if (!hidden) {
        StartSpellCastVisuals(unit, this, &spell, castTime, false);
    }

    if (caster == CGUnit_C::s_activeMover && castTime) {
        SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell.m_spellMissileID);

        if (missile && (missile->m_flags & 0x1)) {
            unit->StartUpdatingMissileTrajectory(this->m_data.m_spellId, OsGetAsyncTimeMs(), castTime);
        }
    }

    if (!hidden) {
        WOWGUID guid = unit->m_obj->m_guid;
        UnitCombatLogCastStart(guid, this->m_data.m_objectTarget, this->m_data.m_spellId);
    }

    if (spell.m_attributes & 0x2) {
        unit->UpdateAmmoModel(ammoDisplayId, 30);
    }

    if (unit->IsActivePlayer()) {
        unit->ReleaseMissiles();
        WOWGUID target;

        if ((spell.m_attributes & 0x400000) && (this->m_data.m_targetFlags & 0x2) && this->m_data.m_objectTarget.m_guid) {
            target = this->m_data.m_objectTarget;
        } else {
            target = CGGameUI::m_lockedTarget;
        }

        if (spell.m_attributes & 0x400000) {
            unit->SetTrackingTarget(target, 0);
        }
    } else if ((this->m_data.m_targetFlags & 0x2) && this->m_data.m_objectTarget.m_guid && this->m_data.m_objectTarget == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && !unit->CanAssist(player, false)) {
            WOWGUID guid = unit->m_obj->m_guid;
            TargetAttacker(guid);
        }
    }
}

// VERIFY: ccbce7b4 manual
// OFFSET: 0x0080FEE0
int32_t Packet_Group_30(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID castItem;
    SmartGUID caster;
    *msg >> castItem;
    *msg >> caster;
    uint8_t castCount;
    msg->Get(castCount);
    uint32_t spellId;
    msg->Get(spellId);
    uint32_t castFlags;
    msg->Get(castFlags);
    PendingSpellCast local;
    PendingSpellCast* cast = GetPendingSpellCast(castCount, spellId, caster);

    if (!cast) {
        if (caster.m_guid && caster == ClntObjMgrGetActivePlayer()) {
            cast = CreatePendingSpellCast(false);
        } else {
            cast = &local;
        }

        cast->m_data.m_castCount = castCount;
        cast->m_data.m_spellId = spellId;
        cast->m_data.m_castItem = castItem;
        cast->m_data.m_caster = caster;
    }

    cast->m_data.m_castFlags = castFlags;

    if (msgId == SMSG_SPELL_START) {
        cast->OnSpellStartPacket(msg);
    } else if (msgId == SMSG_SPELL_GO) {
        cast->OnSpellGo(msg);
    }

    return 1;
}

// VERIFY: 2ad579bf manual
// OFFSET: 0x00722C50
void CGUnit_C::SetPredictedPower(int32_t powerType, int32_t value) {
    uint32_t maximum = powerType == -2 ? this->m_unit->m_unitFieldMaxhealth : this->m_unit->m_unitFieldMaxpower[powerType];

    if (static_cast<int32_t>(maximum) < value) {
        value = maximum;
    }

    if (value == this->m_predictedPower[powerType]) {
        return;
    }

    this->m_predictedPower[powerType] = value;

    if (static_cast<uint32_t>(value) == maximum) {
        Script_SendUnitSignal(this->m_obj->m_guid, powerType + 19);
    }

    if (this->m_obj->m_guid == ClntObjMgrGetActivePlayer() || this->m_obj->m_guid == CGPetInfo::GetPet(0)) {
        CGSpellBook::UpdateUsablePower();
    }
}

// VERIFY: 93438464 manual
// OFFSET: 0x0072E240
void CGUnit_C::ReleaseMissiles() {
    C3Vector position;
    position = this->GetMissileFirePosition();
    this->ReleaseMissiles(position);
}

// VERIFY: 936f6df8 manual
// OFFSET: 0x007FD970
bool Spell_C_GetSpellModifiers(const SpellRec* spell, int32_t modifier, int32_t& flat, int32_t& pct) {
    if (!spell->m_spellClassSet || spell->m_spellClassSet != s_spellClassSet) {
        flat = 0;
        pct = 100;
        return false;
    }

    if (spell->m_attributesExC & 0x20000000) {
        flat = 0;
        pct = 100;
        return false;
    }

    pct = 0;
    flat = 0;

    for (uint32_t i = 0; i < sizeof(spell->m_spellClassMask) * 8; i++) {
        if ((1 << (i & 31)) & spell->m_spellClassMask[i >> 5]) {
            flat += s_spellFlatModifiers[i][modifier];
            pct += s_spellPctModifiers[i][modifier];
        }
    }

    if (!flat && !pct) {
        return false;
    }

    pct += 100;

    if (pct < 0) {
        pct = 0;
    }

    return true;
}

// VERIFY: 8fe30e90 manual
// OFFSET: 0x007FDB50
bool Spell_C_ModifySpellValue(const SpellRec* spell, int32_t& value, int32_t modifier) {
    int32_t flat;
    int32_t pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = pct * (flat + value) / 100;
    return true;
}

// VERIFY: a04d6139 auto
// OFFSET: 0x007FD790
CMapEntity* Spell_C_GetSpellWorldObject() {
    return s_spellWorldObject;
}

// VERIFY: 91de1c39 auto
// OFFSET: 0x007FCC30
void Spell_C_DestroySpellWorldObject() {
    if (s_spellWorldObject) {
        World::ObjectDestroy(s_spellWorldObject);
        s_spellWorldObject = nullptr;
    }

    if (s_spellWorldModel) {
        s_spellWorldModel->Release();
        s_spellWorldModel = nullptr;
    }
}

// VERIFY: decfd300 manual
// OFFSET: 0x007FECC0
void HandleSpellFailedOrGo(CGUnit_C* unit, PendingSpellCast* cast, const SpellRec* spell, int32_t reason) {
    if (!unit) {
        return;
    }

    if (spell && spell->m_spellVisualID[0] > 0 && unit->m_worldModel && unit->m_worldModel->IsLoaded(0, 0) && unit->m_torsoKeyBone != -1) {
        SpellVisualRec* visual = g_spellVisualDB.GetRecord(spell->m_spellVisualID[0]);

        if (visual && visual->m_precastKit > 0) {
            uint32_t sequenceId = unit->m_worldModel->GetBoneSequenceId(unit->m_torsoKeyBone);
            SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit);

            if (kit && kit->m_animID > 0 && sequenceId != 0xFFFFFFFF && static_cast<uint32_t>(kit->m_animID) == sequenceId) {
                unit->OnKeyBoneAnimEnd(unit->m_worldModel, unit->m_torsoKeyBone, kit->m_animID);
            }
        }
    }

    uint8_t castCount = unit->m_castCount;

    if (cast->m_data.m_castCount == castCount && cast->m_data.m_spellId == unit->m_castSpellId) {
        ClearSpellCastState(unit, cast, spell);
    }

    switch (reason) {
        case 187:
            Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 330);
            break;
        case 24:
        case 27:
        case 105:
            Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 327);
            break;
        case 40:
        case 41:
            Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 328);
            break;
        default:
            Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 326);
            break;
    }

    if (cast->m_data.m_castCount == castCount && cast->m_data.m_spellId == static_cast<int32_t>(unit->m_castingSpellId)) {
        unit->ResetUICastingSpell();
        Script_SendUnitSpellSignal(unit->m_obj->m_guid, cast->m_data.m_castCount, spell, 325);
    }

    if (cast == s_targetingSpell) {
        s_needTargets = 0;
        s_targetingSpell = nullptr;
        CursorSetResetMode(1);
        CursorSetMode(1);
        Spell_C_DestroySpellWorldObject();
        CGInputControl* input = CGInputControl::GetActive();
        input->m_mouseModeFlags &= ~0x10;
        input->UpdateMouseMode(0);
    }

    if (reason != 187 && cast->m_data.m_castCount && spell->m_spellMissileID > 0) {
        SpellMissileRec* missile = g_spellMissileDB.GetRecord(spell->m_spellMissileID);

        if (missile && (missile->m_flags & 0x1)) {
            unit->ClearPrecastMissileModel(cast->m_data.m_castCount);
            CMissile::StartFizzling(cast->m_data.m_caster, cast->m_data.m_castCount, cast->m_data.m_spellId);
        }
    }
}

// VERIFY: 107a9a37 manual
// OFFSET: 0x008054F0
void FinishPendingSpellCast(PendingSpellCast* cast, const SpellRec* spell, CGUnit_C* unit) {
    bool playerCast = false;

    if (cast->m_data.m_caster == ClntObjMgrGetActivePlayer() || cast->IsCharmedUnitCast()) {
        playerCast = true;

        if (spell->m_modalNextSpell) {
            int32_t nextSpell = spell->m_modalNextSpell;

            if (nextSpell == GetModalNextSpell()) {
                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                if (player && !player->IsInMelee() && !player->m_unit->m_unitFieldCharmedby.m_guid) {
                    player->OnAttackIconPressed(WOWGUID{ 0 }, 0);
                }
            } else {
                Spell_C_CastSpell(unit, nextSpell, nullptr, cast->m_data.m_objectTarget, nullptr, false);
            }
        }
    }

    ReleasePendingSpellCast(cast);

    if (playerCast && cast->m_data.m_castCount) {
        CGSpellBook::UpdateSelection();
    }
}

// VERIFY: 840e25a5 manual
// OFFSET: 0x00801220
bool Spell_C_HasCooldownOnEvent(const SpellRec* spell, WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (item && (item->m_obj->m_type & TYPEMASK_ITEM)) {
        CGUnit_C* owner = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_UNIT, __FILE__, __LINE__));
        ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, WOWGUID{ 0 }, nullptr, nullptr, false);
        int32_t slot = item->GetOnUseSlot();

        if (stats && owner && slot >= 0) {
            SpellCategoryRec* category = g_spellCategoryDB.GetRecord(stats->m_spellCategory[slot]);

            if (category && (category->m_flags & 0x4)) {
                return true;
            }
        }
    }

    return (spell->m_attributes & 0x2000000) != 0;
}

// VERIFY: 13e43b71 manual
// OFFSET: 0x007FEF60
void Spell_C_ModifyCategoryRecovery(int32_t& recovery, const SpellRec* spell, int32_t pet) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (!pet) {
        SpellCategoryRec* category = g_spellCategoryDB.GetRecord(spell->m_category);

        if (category && (category->m_flags & 0x1)) {
            CGItem_C* item = player->GetBag()->GetItemPointer(15);

            if (item) {
                ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, WOWGUID{ 0 }, nullptr, nullptr, false);

                if (stats) {
                    recovery = recovery * static_cast<int32_t>(stats->m_delay) / 1000;
                }
            }
        }

        if ((spell->m_attributes & 0x2) && !(spell->m_attributesExB & 0x20000)) {
            recovery += player->m_unit->m_unitFieldRangedattacktime;
        }
    }

    if (!(spell->m_attributesExF & 0x80000000)) {
        Spell_C_ModifySpellValue(spell, recovery, 11);
    }
}

// VERIFY: fb43827f manual
// OFFSET: 0x00805230
void SpellHistory::AddHistory(int32_t spellId, int32_t itemId, uint32_t time, uint32_t recoveryTime, int32_t category, uint32_t categoryTime, uint32_t categoryRecoveryTime, bool onHold, int32_t startRecoveryCategory, uint32_t startRecoveryTime) {
    if (!recoveryTime && !categoryRecoveryTime && !onHold && !startRecoveryTime) {
        return;
    }

    SPELLHISTORY* history = this->m_free.Head();

    if (history) {
        this->m_free.UnlinkNode(history);
        this->m_history.LinkToTail(history);
    } else {
        history = this->m_history.NewNode(2, 0, 8);
    }

    history->m_spellId = spellId;
    history->m_itemId = itemId;
    history->m_time = time;
    history->m_recoveryTime = recoveryTime;
    history->m_category = category;
    history->m_categoryTime = categoryTime;
    history->m_categoryRecoveryTime = categoryRecoveryTime;
    history->m_onHold = onHold;
    history->m_startRecoveryCategory = startRecoveryCategory;
    history->m_startRecoveryTime = startRecoveryTime;
}

// VERIFY: ab11e157 manual
// OFFSET: 0x00802970
void SpellHistory::ClearOnHold(int32_t spellId, uint32_t time, bool remove) {
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_spellId != spellId) {
            continue;
        }

        if (history->m_onHold && !remove) {
            history->m_startRecoveryTime = 0;
            history->m_time = time;
            history->m_categoryTime = time;
            history->m_onHold = 0;
        } else if (remove) {
            history->m_link.Unlink();
            this->m_free.LinkToTail(history);
        }
    }
}

// VERIFY: b0e39839 manual
// OFFSET: 0x00809C70
int32_t Packet_SMSG_SPELL_FAILURE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID caster;
    *msg >> caster;
    uint8_t castCount;
    msg->Get(castCount);
    uint32_t spellId;
    msg->Get(spellId);
    uint8_t reason;
    msg->Get(reason);
    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return 1;
    }

    PendingSpellCast local;
    PendingSpellCast* cast = FindPendingSpellCast(caster, &local, castCount, spellId, caster);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        SI2::PlaySpellFizzleSound(spellId, unit);

        if (unit->IsActivePlayer() && (reason == 52 || reason == 53 || reason == 54)) {
            Spell_C_CancelAutoRepeat(true);
        }

        unit->SpellInterrupted(cast->m_data.m_castCount, cast->m_data.m_spellId);
    }

    CGUnit_C* casterUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(cast->m_data.m_caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (casterUnit) {
        HandleSpellFailedOrGo(casterUnit, cast, &spell, reason);
        FinishPendingSpellCast(cast, &spell, casterUnit);
    }

    return 1;
}

// VERIFY: b5828213 manual
// OFFSET: 0x00806DD0
int32_t Packet_SMSG_SPELL_COOLDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    bool startRecovery = false;
    bool ignoreEvent = false;
    WOWGUID guid;
    msg->Get(guid.m_guid);
    uint8_t flags;
    msg->Get(flags);

    if (flags & 0x1) {
        startRecovery = true;
    }

    if (flags & 0x2) {
        ignoreEvent = true;
    }

    int32_t pet;

    if (guid == ClntObjMgrGetActivePlayer()) {
        pet = 0;
    } else {
        if (guid != CGPetInfo::GetPet(0)) {
            return 1;
        }

        pet = 1;
    }

    while (!msg->IsRead()) {
        uint32_t spellId;
        msg->Get(spellId);
        uint32_t cooldown;
        msg->Get(cooldown);
        SpellRec spell;

        if (!g_spellDB.GetRecord(spellId, &spell)) {
            continue;
        }

        bool onHold = ignoreEvent ? false : Spell_C_HasCooldownOnEvent(&spell, guid);
        int32_t flat;
        int32_t pct;
        int32_t recoveryTime;

        if (cooldown) {
            recoveryTime = cooldown;
        } else {
            recoveryTime = spell.m_recoveryTime;

            if (Spell_C_GetSpellModifiers(&spell, 11, flat, pct)) {
                recoveryTime = pct * (recoveryTime + flat) / 100;
            }
        }

        int32_t category = spell.m_category;
        int32_t categoryRecoveryTime;

        if (cooldown) {
            categoryRecoveryTime = 0;
        } else {
            categoryRecoveryTime = spell.m_categoryRecoveryTime;
            Spell_C_ModifyCategoryRecovery(categoryRecoveryTime, &spell, pet);
        }

        int32_t startRecoveryCategory;
        int32_t startRecoveryTime;

        if (!startRecovery || onHold) {
            startRecoveryCategory = 0;
            startRecoveryTime = 0;
        } else {
            startRecoveryCategory = spell.m_startRecoveryCategory;
            startRecoveryTime = spell.m_startRecoveryTime;

            if (Spell_C_GetSpellModifiers(&spell, 21, flat, pct)) {
                startRecoveryTime = pct * (startRecoveryTime + flat) / 100;
            }
        }

        s_spellHistory[pet].AddHistory(spellId, 0, time, recoveryTime, category, time, categoryRecoveryTime, onHold, startRecoveryCategory, startRecoveryTime);
    }

    CGSpellBook::UpdateCooldowns();

    if (pet) {
        CGPetInfo::UpdateCooldowns();
    } else {
        CGContainerInfo::UpdateCooldowns();
    }

    return 1;
}

// VERIFY: e66e3d66 manual
// OFFSET: 0x00809AF0
int32_t Packet_SMSG_CAST_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID player = ClntObjMgrGetActivePlayer();
    uint8_t castCount;
    msg->Get(castCount);
    uint32_t spellId;
    msg->Get(spellId);
    uint8_t reason;
    msg->Get(reason);
    int32_t customError = -1;
    int32_t customArg = -1;

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(customError));
    }

    if (msg->m_read < msg->m_size) {
        msg->Get(reinterpret_cast<uint32_t&>(customArg));
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return 1;
    }

    PendingSpellCast local;
    PendingSpellCast* cast = FindPendingSpellCast(player, &local, castCount, spellId, player);
    Spell_C_SpellFailed(cast, &spell, reason, customError, customArg, true);

    if (reason != 67 && (spell.m_attributes & 0x2000000)) {
        s_spellHistory[0].ClearOnHold(spellId, 0, true);
        CGSpellBook::UpdateCooldowns();
        CGSpellBook::UpdateUsable();
        CGContainerInfo::UpdateCooldowns();
    }

    return 1;
}

// VERIFY: ca363f59 manual
// OFFSET: 0x007FD830
bool PendingSpellCast::HasValidTransports() const {
    C44Matrix matrix;

    if (this->m_data.m_destTarget.m_guid && !MovementGetTransportMtxSafe(this->m_data.m_destTarget, &matrix)) {
        return false;
    }

    if (this->m_data.m_sourceTarget.m_guid && !MovementGetTransportMtxSafe(this->m_data.m_sourceTarget, &matrix)) {
        return false;
    }

    return true;
}

// VERIFY: a5f8984e manual
// OFFSET: 0x00804B20
void Spell_C_AddToInTransitList(WOWGUID caster, const SpellRec* spell, uint32_t serverTime, uint32_t arrivalTime) {
    if (!spell || spell->m_speed <= 0.0f) {
        return;
    }

    void* memory = SMemAlloc(sizeof(SpellInTransit), __FILE__, __LINE__, 0);
    SpellInTransit* node = memory ? new (memory) SpellInTransit() : nullptr;
    node->m_caster = caster;
    node->m_spellId = spell->m_id;
    node->m_serverTime = serverTime;
    node->m_arrivalTime = arrivalTime;
    SpellInTransit* before = nullptr;

    for (SpellInTransit* entry = s_spellsInTransit.Head(); entry; entry = s_spellsInTransit.Next(entry)) {
        if (static_cast<int32_t>(entry->m_arrivalTime - arrivalTime) >= 0) {
            before = entry;
            break;
        }
    }

    s_spellsInTransit.LinkNode(node, 2, before);
}

// VERIFY: 7d01b8cd manual
// OFFSET: 0x00802B00
void SpellHistory::GarbageCollect(uint32_t time) {
    SPELLHISTORY* next;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = next) {
        next = this->m_history.Next(history);

        if (history->m_onHold) {
            continue;
        }

        if (history->m_recoveryTime && static_cast<int32_t>(time - history->m_time - history->m_recoveryTime) < 0) {
            continue;
        }

        if (history->m_categoryRecoveryTime && static_cast<int32_t>(time - history->m_categoryTime - history->m_categoryRecoveryTime) < 0) {
            continue;
        }

        history->m_link.Unlink();
        this->m_free.LinkToTail(history);
    }
}

// VERIFY: 41117beb manual
// OFFSET: 0x0080E1B0
void PendingSpellCast::OnSpellGo(CDataStore* msg) {
    WOWGUID castItem = this->m_data.m_castItem;
    WOWGUID caster = this->m_data.m_caster;
    int32_t spellId = this->m_data.m_spellId;
    uint32_t castFlags = this->m_data.m_castFlags;
    uint32_t serverTime = 0;
    msg->Get(serverTime);

    uint8_t hitCount = 0;
    msg->Get(hitCount);
    TSStackArray<WOWGUID> hits(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * hitCount)), hitCount, 1);

    for (uint32_t i = 0; i < hitCount; i++) {
        msg->Get(hits.m_data[i].m_guid);
    }

    uint8_t missCount = 0;
    msg->Get(missCount);
    TSStackArray<WOWGUID> misses(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * missCount)), missCount, 1);
    TSStackArray<uint32_t> missReasons(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * missCount)), missCount, 1);
    TSStackArray<uint32_t> reflectResults(static_cast<uint32_t*>(alloca(sizeof(uint32_t) * missCount)), missCount, 1);

    for (uint32_t i = 0; i < missCount; i++) {
        msg->Get(misses.m_data[i].m_guid);
        uint8_t value = 0;
        msg->Get(value);
        missReasons.m_data[i] = value;

        if (missReasons.m_data[i] == 11) {
            value = 0;
            msg->Get(value);
            reflectResults.m_data[i] = value;
        }
    }

    SpellGetCastTargets(&this->m_data, msg);
    uint32_t power = 0;
    uint32_t hasPower = castFlags & 0x800;

    if (hasPower) {
        msg->Get(power);
    }

    uint8_t runeCooldowns[6] = { 0, 0, 0, 0, 0, 0 };
    uint8_t runesBefore = 0;
    uint8_t runesAfter = 0;
    uint32_t hasRunes = castFlags & 0x200000;

    if (hasRunes) {
        msg->Get(runesBefore);
        msg->Get(runesAfter);

        for (uint32_t i = 0; i < 6; i++) {
            if (((1 << i) & runesBefore) && !((1 << i) & runesAfter)) {
                msg->Get(runeCooldowns[i]);
            }
        }
    }

    uint32_t delay = 0;

    if (castFlags & 0x20000) {
        msg->Get(this->m_data.m_elevation);
        msg->Get(delay);
    }

    uint32_t ammoDisplayId;
    uint32_t ammoInventoryType;

    if (castFlags & 0x20) {
        msg->Get(ammoDisplayId);
        msg->Get(ammoInventoryType);
    } else {
        ammoDisplayId = 0;
        ammoInventoryType = 0;
    }

    uint32_t visualA = 0;
    uint32_t visualB = 0;

    if (castFlags & 0x80000) {
        msg->Get(visualA);
        msg->Get(visualB);
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (this->m_data.m_targetFlags & 0x40) {
        uint8_t count = 0;
        msg->Get(count);

        if (count) {
            Spell_C_AddProcessedDestLocSpellCast(caster, count, now);
        }
    }

    uint32_t pointCount = 0;

    if (this->m_data.m_targetFlags & 0x80000) {
        msg->Get(pointCount);
    }

    TSStackArray<SpellCastTargetPoint> points(static_cast<SpellCastTargetPoint*>(alloca(sizeof(SpellCastTargetPoint) * pointCount)), pointCount, 1);

    if (this->m_data.m_targetFlags & 0x80000) {
        uint32_t valid = 0;

        for (uint32_t i = 0; i < pointCount; i++) {
            SpellCastTargetPoint& point = points.m_data[valid];
            msg->GetArray(reinterpret_cast<uint32_t*>(&point.m_position), 3);
            msg->Get(point.m_transport.m_guid);

            if (point.m_transport.m_guid) {
                C44Matrix matrix;

                if (!MovementGetTransportMtxX(point.m_transport, &matrix, WOWGUID{ 0 }, __FILE__, __LINE__)) {
                    continue;
                }

                point.m_worldPosition = point.m_position * matrix;
            } else {
                point.m_worldPosition = point.m_position;
            }

            valid++;
        }

        points.SetCount(valid);
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(this->m_data.m_spellId, &spell)) {
        ReleasePendingSpellCast(this);
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!this->HasValidTransports()) {
        if (!(this->m_data.m_castFlags & 0x1) && unit) {
            FinishPendingSpellCast(this, &spell, unit);
        } else {
            ReleasePendingSpellCast(this);
        }

        return;
    }

    if (unit && hasPower) {
        if (spell.m_powerType == -2) {
            if (!(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
                unit->SetPredictedHealth(power);
            }
        } else if (spell.m_powerType == 5) {
            if (caster == ClntObjMgrGetActivePlayer() && hasRunes) {
                CGRuneInfo::SetPredictedRunes(runesBefore, runesAfter, runeCooldowns);
            }
        } else {
            if (!(unit->m_unit->m_unitFieldFlags2 & 0x100)) {
                unit->SetPredictedPower(spell.m_powerType, power);
            }

            if (spell.m_powerType == 0) {
                unit->SetManaRegenInterrupt();
            }
        }
    }

    uint32_t hidden = castFlags & 0x1;

    if (!hidden) {
        unk_D397C4 = 0;
        HandleSpellFailedOrGo(unit, this, &spell, 187);
    }

    uint32_t arrival = now;
    C3Vector destLocation = this->m_data.GetDestLocation();

    if (unit && spell.m_speed != 0.0f) {
        if (this->m_data.m_targetFlags & 0x40) {
            C3Vector position = unit->GetPosition();
            float dx = position.x - destLocation.x;
            float dy = position.y - destLocation.y;
            float dz = position.z - destLocation.z;
            arrival = now - static_cast<int32_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell.m_speed * -1000.0f);
            Spell_C_AddToInTransitList(caster, &spell, serverTime, arrival);
        } else if (hitCount) {
            CGObject_C* target = ClntObjMgrObjectPtr(hits.m_data[0], TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (target) {
                C3Vector targetPosition = target->GetPosition();
                C3Vector position = unit->GetPosition();
                float dx = position.x - targetPosition.x;
                float dy = position.y - targetPosition.y;
                float dz = position.z - targetPosition.z;
                arrival = now - static_cast<int32_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell.m_speed * -1000.0f);
                Spell_C_AddToInTransitList(caster, &spell, serverTime, arrival);
            }
        }

        if (this->m_data.m_targetFlags & 0x80000) {
            for (uint32_t i = 0; i < points.m_count; i++) {
                const SpellCastTargetPoint& point = points.m_data[i];
                C3Vector position = unit->GetPosition();
                float dx = position.x - point.m_worldPosition.x;
                float dy = position.y - point.m_worldPosition.y;
                float dz = position.z - point.m_worldPosition.z;
                arrival = now - static_cast<int32_t>(sqrtf(dx * dx + dy * dy + dz * dz) / spell.m_speed * -1000.0f);
                Spell_C_AddToInTransitList(caster, &spell, serverTime, arrival);
            }
        }
    }

    CGObject_C* castObject = ClntObjMgrObjectPtr(castItem, TYPEMASK_OBJECT, __FILE__, __LINE__);
    WOWGUID source = castObject && (castObject->m_obj->m_type & (TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT)) ? castItem : caster;

    if (!hidden || (spell.m_attributesExG & 0x80000000)) {
        UnitCombatLogCastGo(caster, this->m_data.m_objectTarget, spellId);
    } else if (spell.m_attributesExF & 0x40) {
        UnitCombatLogProcGo(castItem, &spell);
    }

    if (spell.m_speed == 0.0f && !(spell.m_attributesExB & 0x80)) {
        for (uint32_t i = 0; i < missCount; i++) {
            UnitCombatLogSpellMissed(source, misses.m_data[i], spellId, missReasons.m_data[i], false, false, 0);

            if (!(spell.m_attributes & 0x100)) {
                CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(misses.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

                if (target) {
                    target->AddWorldText(source, missReasons.m_data[i], &spell, false);
                }

                CGGameUI::ShowSpellMissFeedback(misses.m_data[i], missReasons.m_data[i]);
            }

            if (missReasons.m_data[i] == 11 && reflectResults.m_data[i]) {
                UnitCombatLogSpellMissed(source, caster, spellId, reflectResults.m_data[i], false, false, 0);

                if (!(spell.m_attributes & 0x100)) {
                    CGUnit_C* reflector = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (reflector) {
                        reflector->AddWorldText(source, reflectResults.m_data[i], &spell, false);
                    }

                    CGGameUI::ShowSpellMissFeedback(caster, reflectResults.m_data[i]);
                }
            }

            CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(misses.m_data[i], TYPEMASK_UNIT, __FILE__, __LINE__));

            if (target) {
                target->PlayCombatSpellMissEffects(missReasons.m_data[i], source);
            }
        }
    }

    bool onHold = Spell_C_HasCooldownOnEvent(&spell, castItem);
    TSStackArray<WOWGUID> targets(static_cast<WOWGUID*>(alloca(sizeof(WOWGUID) * (missCount + hitCount))), missCount + hitCount, 1);

    if (castFlags & 0x400) {
        for (uint32_t i = 0; i < missCount; i++) {
            targets.m_data[i] = misses.m_data[i];
        }

        for (uint32_t i = 0; i < hitCount; i++) {
            targets.m_data[missCount + i] = hits.m_data[i];
        }
    } else {
        for (uint32_t i = 0; i < hitCount; i++) {
            targets.m_data[i] = hits.m_data[i];
        }

        for (uint32_t i = 0; i < missCount; i++) {
            targets.m_data[hitCount + i] = misses.m_data[i];
        }
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual;

    if (unit) {
        visual = unit->GetAppropriateSpellVisual(&spell, visualRec, visualA, visualB);

        if (visual && Spell_C_SpellVisualHasChainEffect(visual)) {
            unit->SaveChainSpellTargets(targets);
            bool chainLocation = (visual->m_flags & 0x1) && targets.m_count;

            if ((this->m_data.m_targetFlags & 0x40) && !chainLocation) {
                unit->SaveChainSpellLocation(&destLocation);
            } else {
                unit->SaveChainSpellLocation(nullptr);
            }
        }
    } else {
        visual = GetSpellVisual(&spell);
    }

    if (visual) {
        SpellVisualKitRec* castKit = g_spellVisualKitDB.GetRecord(visual->m_castKit);

        if (castKit && unit && !this->unk_0139) {
            PlaySpellVisuaKitData kitData(&spell, castKit, 0);
            unit->PlaySpellVisualKit(kitData);
        }

        if (this->m_data.m_targetFlags & 0x40) {
            SpellVisualKitRec* instantKit = g_spellVisualKitDB.GetRecord(visual->m_instantAreaKit);

            if (instantKit && unit) {
                PlaySpellVisuaKitData kitData(&spell, instantKit, 3);
                kitData.m_position = &this->m_data.m_destPos;
                kitData.m_transportGUID = this->m_data.m_destTarget;
                unit->PlaySpellVisualKit(kitData);
            }

            int32_t impactKitId = visual->m_impactAreaKit;
            SpellVisualKitRec* impactKit = g_spellVisualKitDB.GetRecord(impactKitId);

            if (impactKit && (!visual->m_hasMissile || spell.m_speed <= 0.0f) && !Spell_C_SpellVisualHasChainEffect(visual)) {
                if (unit) {
                    if (spell.m_speed > 0.0f) {
                        unit->DelaySpellVisualKit(spellId, impactKitId, 3, &this->m_data.m_destPos, this->m_data.m_destTarget, 0, 0, -1, 1, arrival);
                    } else {
                        PlaySpellVisuaKitData kitData(&spell, impactKit, 3);
                        kitData.m_position = &this->m_data.m_destPos;
                        kitData.m_transportGUID = this->m_data.m_destTarget;
                        unit->PlaySpellVisualKit(kitData);
                    }
                } else if (ClntObjMgrObjectPtr(castItem, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__)) {
                    NotifyDestLocSpellCastData data;
                    data.m_caster = castItem;
                    data.m_destTarget = this->m_data.m_destTarget;
                    data.m_spellId = spellId;
                    data.m_destLocation = destLocation;
                    data.m_destPosition = this->m_data.m_destPos;
                    data.m_elevation = this->m_data.m_elevation;
                    data.unk_0030 = this->m_data.unk_0120;
                    data.unk_0034 = 0;
                    data.unk_0038 = 0;
                    data.m_castCount = this->m_data.m_castCount;
                    Spell_C_PlayDestLocVisualsWithoutCaster(&data);
                }
            }
        }

        if (this->m_data.m_targetFlags & 0x80000) {
            for (uint32_t i = 0; i < points.m_count; i++) {
                SpellCastTargetPoint& point = points.m_data[i];
                SpellVisualKitRec* instantKit = g_spellVisualKitDB.GetRecord(visual->m_instantAreaKit);

                if (instantKit && unit) {
                    PlaySpellVisuaKitData kitData(&spell, instantKit, 3);
                    kitData.m_position = &point.m_position;
                    kitData.m_transportGUID = point.m_transport;
                    unit->PlaySpellVisualKit(kitData);
                }

                int32_t impactKitId = visual->m_impactAreaKit;
                SpellVisualKitRec* impactKit = g_spellVisualKitDB.GetRecord(impactKitId);

                if (!impactKit || (visual->m_hasMissile && spell.m_speed > 0.0f) || Spell_C_SpellVisualHasChainEffect(visual)) {
                    continue;
                }

                if (unit) {
                    if (spell.m_speed > 0.0f) {
                        unit->DelaySpellVisualKit(spellId, impactKitId, 3, &point.m_position, point.m_transport, 0, 0, -1, 1, arrival);
                    } else {
                        PlaySpellVisuaKitData kitData(&spell, impactKit, 3);
                        kitData.m_position = &point.m_position;
                        kitData.m_transportGUID = point.m_transport;
                        unit->PlaySpellVisualKit(kitData);
                    }
                } else if (ClntObjMgrObjectPtr(castItem, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__)) {
                    NotifyDestLocSpellCastData data;
                    data.m_caster = castItem;
                    data.m_destTarget = WOWGUID{ 0 };
                    data.m_spellId = spellId;
                    data.m_destLocation = point.m_worldPosition;
                    data.m_destPosition = point.m_worldPosition;
                    data.m_elevation = this->m_data.m_elevation;
                    data.unk_0030 = this->m_data.unk_0120;
                    data.unk_0034 = 0;
                    data.unk_0038 = 0;
                    data.m_castCount = this->m_data.m_castCount;
                    Spell_C_PlayDestLocVisualsWithoutCaster(&data);
                }
            }
        }

        if (spell.m_speed <= 0.0f) {
            PlayImpactVisual(&spell, hits, unit, visual);

            if (unit) {
                for (uint32_t i = 0; i < missCount; i++) {
                    if (missReasons.m_data[i] == 11 && !reflectResults.m_data[i] && !(spell.m_attributesExG & 0x20000000)) {
                        PlayImpactVisualOnTarget(&spell, visual, unit, unit);
                    }
                }
            }
        } else if (unit) {
            if (this->unk_0139) {
                if (this->m_data.m_castFlags & 0x20000) {
                    CMissile::AdjustAllMissileTrajectoryDurations(this->m_data.m_castCount, delay);
                }
            } else {
                if (spell.m_attributesExD & 0x40000) {
                    LaunchMissileVisual(hits, misses, unit, &spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, missReasons, reflectResults, delay);

                    if ((visual->m_hasMissile || ammoDisplayId) && points.m_count) {
                        unit->CreateMissile(points, &this->m_data, ammoDisplayId, ammoInventoryType, visual, spell.m_id, delay);
                    }
                } else {
                    CreateMissilesForTargets(hits, unit, &spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, nullptr, nullptr, delay);

                    if (misses.m_count) {
                        CreateMissilesForTargets(misses, unit, &spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, &missReasons, &reflectResults, delay);
                    }

                    CreateMissilesForTargetPoints(points, unit, &spell, visual, ammoDisplayId, ammoInventoryType, &this->m_data, delay);
                }

                if (visual->m_hasMissile && (!castKit || castKit->m_animID <= 0)) {
                    unit->ReleaseMissiles();
                }
            }
        }
    }

    if (caster == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(unit);

        if (castItem == ClntObjMgrGetActivePlayer() && !(castFlags & 0x41)) {
            CGTradeSkillInfo::DoRecast(spellId);
        }

        for (uint32_t effect = 0; effect < 3; effect++) {
            if (spell.m_effect[effect] != 33) {
                continue;
            }

            if (hits.m_count != 1) {
                break;
            }

            CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(hits.m_data[0], TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (!gameObject) {
                break;
            }

            int8_t type = gameObject->m_gameObject->m_typeID;

            if (type < 0 || (type > 1 && type != 3)) {
                break;
            }

            if (type == 3) {
                player->OnLootGameObject(hits.m_data[0]);
            }

            int32_t action = -1;
            LockRec* lock = gameObject->GetLockRec();

            if (!lock) {
                break;
            }

            for (uint32_t i = 0; i < 3; i++) {
                if (spell.m_effect[i] != 33) {
                    continue;
                }

                for (uint32_t j = 0; j < 8 && action < 0; j++) {
                    switch (lock->m_type[j]) {
                        case 1:
                            if ((castObject->m_obj->m_type & TYPEMASK_ITEM) && castObject->m_obj->m_entryID == lock->m_index[j]) {
                                action = lock->m_action[j];
                            }

                            break;
                        case 2:
                            if (spell.m_effectMiscValue[i] == lock->m_index[j]) {
                                action = lock->m_action[j];
                            }

                            break;
                        case 3:
                            if (spell.m_id == lock->m_index[j]) {
                                action = lock->m_action[j];
                            }

                            break;
                    }
                }
            }

            if (action >= 0) {
                if (action <= 1) {
                    gameObject->SetDisplayState(0);
                } else if (action == 2) {
                    gameObject->SetDisplayState(1);
                }
            }

            break;
        }

        for (uint32_t i = 0; i < 3; i++) {
            int32_t effect = spell.m_effect[i];

            if (effect == 71 || effect == 95 || effect == 116 || ((effect == 99 || effect == 127 || effect == 158) && CGGameUI::IsAutoLooting())) {
                if (!player->m_autoLoot) {
                    player->SetAutoLoot(g_autoLootDefaultCVar->m_intValue);
                }

                break;
            }
        }

        if (((spell.m_attributesExB & 0x100000) || (((spell.m_attributes & 0x404) || (spell.m_attributesEx & 0x200) || (spell.m_attributesExB & 0x100000)) && (spell.m_interruptFlags & 0x8))) && !player->m_combat.m_attacking.m_guid) {
            player->OnAttackIconPressed(hits.m_count ? hits.m_data[0] : WOWGUID{ 0 }, 0);
        }

        int32_t itemId = 0;
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(castItem, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            itemId = item->m_obj->m_entryID;
        }

        if (castItem == caster) {
            int32_t categoryRecovery = spell.m_categoryRecoveryTime;
            Spell_C_ModifyCategoryRecovery(categoryRecovery, &spell, 0);
            int32_t recovery = spell.m_recoveryTime;
            int32_t flat;
            int32_t pct;

            if (Spell_C_GetSpellModifiers(&spell, 11, flat, pct)) {
                recovery = pct * (flat + recovery) / 100;
            }

            s_spellHistory[0].AddHistory(spellId, 0, now, recovery, spell.m_category, now, categoryRecovery, onHold, 0, 0);
            CGSpellBook::UpdateCooldowns();

            if (onHold) {
                CGSpellBook::UpdateUsable();
            }

            CGContainerInfo::UpdateCooldowns();
        } else if (itemId) {
            if (!g_itemDBCache.GetRecord(itemId, castItem, &Spell_C_PendingItemCooldownCallback, reinterpret_cast<void*>(1), false)) {
                Spell_C_AddPendingItemCooldown(itemId, spellId, now, onHold);
                return;
            }

            if (item->GetEnchantmentSpell(0) == spellId) {
                uint32_t cooldown;
                uint32_t categoryCooldown;
                uint32_t category;
                item->GetSpellCooldownInfo(cooldown, categoryCooldown, category);

                if (static_cast<int32_t>(cooldown) < 0) {
                    cooldown = spell.m_recoveryTime;
                }

                if (static_cast<int32_t>(category) <= 0) {
                    category = spell.m_category;
                }

                if (static_cast<int32_t>(categoryCooldown) < 0) {
                    categoryCooldown = spell.m_categoryRecoveryTime;
                }

                s_spellHistory[0].AddHistory(spellId, itemId, now, cooldown, category, now, categoryCooldown, onHold, 0, 0);
            }

            CGSpellBook::UpdateCooldowns();

            if (onHold) {
                CGSpellBook::UpdateUsable();
            }

            CGContainerInfo::UpdateCooldowns();
        }
    }

    if (unit) {
        const WOWGUID& controller = unit->m_unit->m_unitFieldCharmedby.m_guid ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldSummonedby;

        if (controller == ClntObjMgrGetActivePlayer()) {
            int32_t recovery = spell.m_recoveryTime;
            int32_t categoryRecovery = spell.m_categoryRecoveryTime;
            int32_t flat;
            int32_t pct;

            if (Spell_C_GetSpellModifiers(&spell, 11, flat, pct)) {
                recovery = pct * (flat + recovery) / 100;
            }

            if (!(spell.m_attributesExF & 0x80000000) && Spell_C_GetSpellModifiers(&spell, 11, flat, pct)) {
                categoryRecovery = pct * (flat + spell.m_categoryRecoveryTime) / 100;
            }

            int32_t startRecoveryTime = 0;
            int32_t startRecoveryCategory = 0;

            if (!(this->m_data.m_castFlags & 0x40000)) {
                startRecoveryTime = spell.m_startRecoveryTime;
                startRecoveryCategory = spell.m_startRecoveryCategory;

                if (startRecoveryTime || startRecoveryCategory) {
                    if (startRecoveryCategory == 133 && startRecoveryTime == 1500 && spell.m_defenseType != 2 && spell.m_defenseType != 3 && !(spell.m_attributes & 0x12)) {
                        float scaled = unit->m_unit->m_unitModCastSpeed * 1500.0f;
                        startRecoveryTime = CMath::fint_(scaled - 0.5f);
                    }

                    if (Spell_C_GetSpellModifiers(&spell, 21, flat, pct)) {
                        startRecoveryTime = pct * (startRecoveryTime + flat) / 100;
                    }

                    if (static_cast<uint32_t>(startRecoveryTime - 1000) > 500) {
                        startRecoveryTime = startRecoveryTime - 1000 < 0 ? 1000 : 1500;
                    }
                }
            }

            s_spellHistory[1].AddHistory(spellId, 0, now, recovery, spell.m_category, now, categoryRecovery, onHold, startRecoveryCategory, startRecoveryTime);
            CGSpellBook::UpdateCooldowns();

            if (onHold) {
                CGSpellBook::UpdateUsable();
            }

            CGPetInfo::UpdateCooldowns();
        }
    }

    if (static_cast<int32_t>(now - s_nextHistoryCleanup) >= 0) {
        s_nextHistoryCleanup = now + 120000;

        for (uint32_t i = 0; i < sizeof(s_spellHistory) / sizeof(s_spellHistory[0]); i++) {
            s_spellHistory[i].GarbageCollect(now);
        }
    }

    if (!(this->m_data.m_castFlags & 0x1) && unit) {
        FinishPendingSpellCast(this, &spell, unit);
    } else {
        ReleasePendingSpellCast(this);
    }
}

// VERIFY: 2af0e265 manual
// OFFSET: 0x007FDE00
uint32_t Spell_C_GetPowerDisplayMod(uint32_t powerType) {
    if (static_cast<int32_t>(powerType) < 0) {
        return 1;
    }

    return s_powerDisplayMod[powerType];
}

// VERIFY: f2862d5e auto
// OFFSET: 0x007FE4B0
const char* GetPowerString(uint32_t powerType) {
    const char* tokens[7] = { "MANA", "RAGE", "FOCUS", "ENERGY", "HAPPINESS", "RUNES", "RUNIC_POWER" };

    if (powerType < sizeof(tokens) / sizeof(tokens[0])) {
        return tokens[powerType];
    }

    return "";
}

// VERIFY: a2eb54f0 auto
// OFFSET: 0x007FA290
const SpellVisualRec* GetSpellVisual(const SpellRec* spell) {
    int32_t visualId = spell->m_spellVisualID[0];

    if (ViolenceGetLevel() < 2 && spell->m_spellVisualID[1]) {
        visualId = spell->m_spellVisualID[1];
    }

    return g_spellVisualDB.GetRecord(visualId);
}

// VERIFY: 8f243d8d manual
// OFFSET: 0x007FD750
bool Spell_C_CanTargetTerrain() {
    return (s_needTargets & 0x60) != 0;
}

// VERIFY: 4bd9ca55 auto
// OFFSET: 0x007FD7F0
int32_t Spell_C_IsCursorWorldObjectHousing() {
    return s_cursorWorldObjectHousing;
}

// VERIFY: 61051f38 auto
// OFFSET: 0x007FE1A0
void SpellSetFeedPetSpell(int32_t spellId) {
    unk_D397B8 = spellId;
}

// VERIFY: 54519a8f manual
// OFFSET: 0x007FD650
bool Spell_C_CanTargetUnits() {
    return (s_needTargets & 0x1878E) != 0;
}

// VERIFY: 402f8659 auto
// OFFSET: 0x007FD950
int32_t Packet_SMSG_FEIGN_DEATH_RESISTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_FEIGN_DEATH_RESISTED);
    return 1;
}

// VERIFY: ff87ae53 auto
// OFFSET: 0x007FD800
void Spell_C_CursorWorldObjectRotate() {
    s_cursorWorldObjectFacing = s_cursorWorldObjectFacing + 1.5707964f;

    if (s_cursorWorldObjectFacing >= 6.2831855f) {
        s_cursorWorldObjectFacing = s_cursorWorldObjectFacing - 6.2831855f;
    }
}

// VERIFY: 5da1ec93 auto
// OFFSET: 0x007FDF60
bool SpellIsTrackingAura(const SpellRec* spell) {
    for (int32_t i = 0; i < 3; i++) {
        int32_t aura = spell->m_effectAura[i];

        if (aura == 44 || aura == 45 || aura == 151) {
            return true;
        }
    }

    return false;
}

// VERIFY: a97f4372 manual
// OFFSET: 0x00800510
int32_t Packet_SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_vehiclePassenger) {
        player->m_vehiclePassenger->ClearFlag8();
    }

    return 1;
}

// VERIFY: 9a6eeb12 auto
// OFFSET: 0x008007A0
void Spell_C_SetPlayerClass(int32_t classId) {
    ChrClassesRec* rec = g_chrClassesDB.GetRecord(classId);

    if (rec) {
        s_spellClassSet = rec->m_spellClassSet;
    }
}

// VERIFY: 28235946 manual
// OFFSET: 0x007FD900
int32_t Packet_SMSG_GAMEOBJECT_RESET_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (gameObject) {
        gameObject->ResetDisplayState();
    }

    return 1;
}

// VERIFY: e42c1b1d auto
// OFFSET: 0x007FDC60
int32_t Packet_Group_32(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t group;
    uint8_t op;
    uint32_t value;
    msg->Get(group);
    msg->Get(op);
    msg->Get(value);

    if (msgId == SMSG_SET_PCT_SPELL_MODIFIER) {
        s_spellPctModifiers[group][op] = value;
    } else {
        s_spellFlatModifiers[group][op] = value;
    }

    return 1;
}

// VERIFY: 2541c8b3 manual
// OFFSET: 0x00805FC0
bool Spell_C_IsCurrentItem(int32_t itemId) {
    for (PendingSpellCast* cast = s_pendingSpellCasts.Head(); cast; cast = s_pendingSpellCasts.Next(cast)) {
        if (cast->m_data.m_castItem == cast->m_data.m_caster) {
            continue;
        }

        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cast->m_data.m_castItem, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item && item->m_obj->m_entryID == itemId) {
            return true;
        }
    }

    return false;
}

// VERIFY: 1ec86af2 manual
// OFFSET: 0x00807560
void Spell_C_CancelAutoRepeat(bool a1) {
    if (!s_autoRepeatingSpell) {
        return;
    }

    if (a1) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CANCEL_AUTO_REPEAT_SPELL));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit) {
        unit->SetCastingAutoRepeat(false);
        unit->CancelRangedMode();
    }

    Spell_C_CancelSpellsWithId(s_autoRepeatingSpell);

    if (!s_autoRepeatingSpell) {
        return;
    }

    s_autoRepeatingSpell = 0;

    if (s_autoRangedCombatSpell) {
        return;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_STOP_AUTOREPEAT_SPELL, nullptr);

    if (s_autoRepeatingSpell) {
        static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->MeleeModeLeave();
    }
}

// VERIFY: 2524719c auto
// OFFSET: 0x007FDE50
bool SpellRec::HasAura(const SpellRec* spell, int32_t aura) {
    for (int32_t i = 0; i < 3; i++) {
        if (spell->m_effectAura[i] == aura) {
            return true;
        }
    }

    return false;
}

// VERIFY: 86c68872 auto
// OFFSET: 0x007FE1B0
int32_t SpellRec::IsHelpfulOrHarmful(const SpellRec* spell) {
    if (spell->m_targets & 0x100) {
        return 1;
    }

    if (spell->m_targets & 0x80) {
        return 2;
    }

    for (uint32_t i = 0; i < 3; i++) {
        switch (spell->m_implicitTargetA[i]) {
            case 2:
            case 6:
            case 15:
            case 16:
            case 24:
            case 28:
            case 53:
            case 54:
            case 93:
                return 2;
        }

        switch (spell->m_implicitTargetB[i]) {
            case 2:
            case 6:
            case 15:
            case 16:
            case 24:
            case 28:
            case 53:
            case 54:
            case 93:
                return 2;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        switch (spell->m_implicitTargetA[i]) {
            case 1:
                if (spell->m_effectAura[i] != 4) {
                    return 1;
                }

                break;

            case 3:
            case 4:
            case 5:
            case 20:
            case 21:
            case 27:
            case 29:
            case 30:
            case 31:
            case 33:
            case 34:
            case 35:
            case 45:
            case 56:
            case 57:
            case 58:
            case 59:
            case 61:
            case 62:
                return 1;
        }

        switch (spell->m_implicitTargetB[i]) {
            case 1:
                if (spell->m_effectAura[i] != 4) {
                    return 1;
                }

                break;

            case 3:
            case 4:
            case 5:
            case 20:
            case 21:
            case 27:
            case 29:
            case 30:
            case 31:
            case 33:
            case 34:
            case 35:
            case 45:
            case 56:
            case 57:
            case 58:
            case 59:
            case 61:
            case 62:
                return 1;
        }
    }

    return 0;
}

// VERIFY: 69f73b82 auto
// OFFSET: 0x008009B0
bool SpellRec::WillSummonCritter(const SpellRec* spell) {
    if (spell->m_effect[0] == 28) {
        SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(spell->m_effectMiscValueB[0]);

        if (properties && properties->m_slot == 5) {
            return true;
        }
    }

    return spell->m_effectAura[0] == 78;
}

// VERIFY: 54ab5610 manual
// OFFSET: 0x007FFA30
int32_t Spell_C_GetSpellReagentUses(const SpellRec* spell) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t uses = 0x7FFFFFFF;

    for (uint32_t i = 0; i < 8; i++) {
        int32_t reagent = spell->m_reagent[i];

        if (reagent != -2 && reagent && spell->m_reagentCount[i] > 0) {
            int32_t count = player->GetBag()->GetItemTypeCount(reagent, 0) / spell->m_reagentCount[i];

            if (count < uses) {
                uses = count;
            }
        }
    }

    return uses == 0x7FFFFFFF ? 0 : uses;
}

// VERIFY: 1bbeb20f auto
// OFFSET: 0x00801770
int32_t Spell_C_GetSpellReagentUses(int32_t spellId) {
    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return 0;
    }

    return Spell_C_GetSpellReagentUses(&spell);
}

// VERIFY: db856fff manual
// OFFSET: 0x00807980
bool SpellHistory::GetSpellCooldown(int32_t spellId, int32_t itemId, uint32_t* duration, uint32_t* start, uint32_t* enable) {
    if (enable) {
        *enable = 1;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell) || spell.m_effect[0] == 78 || spell.m_effect[0] == 47) {
        return false;
    }

    int32_t category = spell.m_category;
    int32_t startRecoveryCategory = spell.m_startRecoveryCategory;

    if (itemId) {
        ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, WOWGUID(), nullptr, nullptr, false);

        if (stats) {
            for (uint32_t i = 0; i < 5; i++) {
                if (stats->m_spellId[i] == static_cast<uint32_t>(spellId) && static_cast<int32_t>(stats->m_spellCategory[i]) > 0) {
                    category = stats->m_spellCategory[i];
                }
            }
        }
    }

    uint32_t now = OsGetAsyncTimeMs();
    uint32_t latest = now;

    for (SPELLHISTORY* history = this->m_history.Head(); history; history = this->m_history.Next(history)) {
        if (history->m_spellId == spellId && history->m_itemId == itemId && (history->m_recoveryTime || history->m_onHold)) {
            uint32_t end = history->m_recoveryTime + (history->m_onHold ? now + 1 : history->m_time);

            if (static_cast<int32_t>(end - latest) >= 0) {
                if (duration) {
                    *duration = history->m_onHold ? 1 : history->m_recoveryTime;
                }

                if (start) {
                    *start = history->m_onHold ? now : history->m_time;
                }

                if (enable && *enable) {
                    *enable = history->m_onHold == 0;
                }

                latest = end;
            }
        }

        SpellCategoryRec* categoryRec = g_spellCategoryDB.GetRecord(history->m_category);

        if (((history->m_category && history->m_category == category) || (categoryRec && (categoryRec->m_flags & 0x2))) && history->m_categoryRecoveryTime) {
            uint32_t end = history->m_categoryRecoveryTime + (history->m_onHold ? now : history->m_categoryTime);

            if (static_cast<int32_t>(end - latest) >= 0) {
                if (duration) {
                    *duration = history->m_categoryRecoveryTime;
                }

                if (start) {
                    *start = history->m_onHold ? now : history->m_categoryTime;
                }

                if (enable && *enable) {
                    *enable = history->m_onHold == 0;
                }

                latest = end;
            }
        }

        if (history->m_startRecoveryCategory == startRecoveryCategory && history->m_startRecoveryTime) {
            uint32_t end = history->m_startRecoveryTime + history->m_time;

            if (static_cast<int32_t>(end - latest) >= 0) {
                if (duration) {
                    *duration = history->m_startRecoveryTime;
                }

                if (start) {
                    *start = history->m_time;
                }

                latest = end;
            }
        }

        if (!spell.m_runeCostID) {
            continue;
        }

        bool sharesRunes = spellId == history->m_spellId;

        if (!sharesRunes) {
            SpellRec other;

            if (g_spellDB.GetRecord(history->m_spellId, &other) && other.m_runeCostID) {
                SpellRuneCostRec* cost = g_spellRuneCostDB.GetRecord(spell.m_runeCostID);
                SpellRuneCostRec* otherCost = g_spellRuneCostDB.GetRecord(other.m_runeCostID);

                if (cost && otherCost && ((cost->m_blood && otherCost->m_blood) || (cost->m_unholy && otherCost->m_unholy) || (cost->m_frost && otherCost->m_frost))) {
                    sharesRunes = true;
                }
            }
        }

        uint32_t runeTime;

        if (!sharesRunes || !CGRuneInfo::ComputeRuneSpellCooldown(&spell, &runeTime) || static_cast<int32_t>(runeTime - latest) < 0) {
            continue;
        }

        latest = runeTime;

        if (start) {
            *start = history->m_time;
        }

        if (!duration || !start) {
            continue;
        }

        if (!CGRuneInfo::HasRunesForSpell(spellId, nullptr)) {
            *start = latest - 10000;
            *duration = 10000;
        } else {
            *duration = history->m_startRecoveryTime;
        }
    }

    return latest != now;
}

// VERIFY: a5bd03ff auto
// OFFSET: 0x00809000
bool Spell_C_GetSpellCooldown(int32_t spellId, int32_t petSpell, uint32_t* duration, uint32_t* start, uint32_t* enable) {
    return s_spellHistory[petSpell].GetSpellCooldown(spellId, 0, duration, start, enable);
}

// VERIFY: f18946a2 auto
// OFFSET: 0x007FD440
static bool GetEffectModifierType(int32_t effect, bool* periodic, int32_t aura, bool* rounded) {
    *rounded = false;
    *periodic = false;

    switch (effect) {
        case 2:
        case 9:
        case 10:
        case 17:
        case 31:
        case 58:
        case 67:
        case 75:
        case 121:
            *rounded = true;
            return true;

        case 6:
        case 27:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
            switch (aura) {
                case 3:
                case 8:
                case 20:
                case 53:
                case 62:
                case 89:
                    *rounded = true;
                    *periodic = true;
                    return true;

                case 15:
                case 43:
                    *rounded = true;
                    return true;

                case 21:
                case 24:
                case 63:
                case 64:
                case 162:
                    *rounded = true;
                    return false;
            }

            return false;

        case 8:
        case 30:
        case 62:
        case 146:
            *rounded = true;
            return false;
    }

    return false;
}

// VERIFY: 207d6b5b manual
// OFFSET: 0x007FDBA0
bool Spell_C_ModifySpellValue(const SpellRec* spell, float& value, int32_t modifier) {
    int32_t flat;
    int32_t pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = (flat + value) * pct * 0.01f;
    return true;
}

// VERIFY: f7b1dbf5 manual
// OFFSET: 0x007FDBE0
static bool Spell_C_ModifySpellValue(const SpellRec* spell, CSoftFloat& value, int32_t modifier) {
    int32_t flat;
    int32_t pct;

    if (!Spell_C_GetSpellModifiers(spell, modifier, flat, pct)) {
        return false;
    }

    value = (value + CSoftFloat::FromInt(flat)) * CSoftFloat::FromInt(pct) * s_softPercent;
    return true;
}

// VERIFY: 49227fd2 auto
// OFFSET: 0x007FDE80
const int32_t* SpellRec::GetClassMask(const SpellRec* spell, int32_t effect) {
    switch (effect) {
        case 0:
            return spell->m_effectSpellClassMaskA;

        case 1:
            return spell->m_effectSpellClassMaskB;

        case 2:
            return spell->m_effectSpellClassMaskC;
    }

    return nullptr;
}

// VERIFY: 9c2128c3 auto
// OFFSET: 0x007FDEC0
bool SpellRec::IsAffectedBy(const SpellRec* spell, const SpellRec* modifier, int32_t effect) {
    if (!spell || !modifier || spell->m_spellClassSet != modifier->m_spellClassSet) {
        return false;
    }

    const int32_t* mask = SpellRec::GetClassMask(modifier, effect);

    for (uint32_t i = 0; i < 3; i++) {
        if (mask[i] & spell->m_spellClassMask[i]) {
            return true;
        }
    }

    return false;
}

// VERIFY: b26cb571 manual
// OFFSET: 0x007FE850
bool SpellRec::UsableInShapeshift(const SpellRec* spell, int32_t form) {
    return form >= 0 && (spell->m_shapeshiftMask[form / 32] & (1 << (form & 0x1F)));
}

// VERIFY: 18bb777f manual
// OFFSET: 0x007FF070
uint32_t Spell_C_GetSpellLevel(const SpellRec* spell, int32_t isPet, int32_t isUnit) {
    CGUnit_C* unit;

    if (isUnit) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGCharacterInfo::m_inspectTarget, TYPEMASK_UNIT, __FILE__, __LINE__));
    } else if (isPet) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    } else {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    }

    if (!unit) {
        return 0;
    }

    return static_cast<uint32_t>(unit->GetSpellRank(spell)) / 5;
}

// VERIFY: afcd1587 auto
// OFFSET: 0x007FF100
int32_t Spell_C_GetPowerCostPerSecond(const SpellRec* spell, const CGUnit_C* unit) {
    if (!unit) {
        return 0;
    }

    uint32_t rank = unit->GetSpellRank(spell);
    int32_t cost = spell->m_manaPerSecond + spell->m_manaPerSecondPerLevel * (static_cast<int32_t>(rank / 5) - spell->m_baseLevel);
    int32_t flat;
    int32_t pct;

    if (cost > 0 && Spell_C_GetSpellModifiers(spell, 14, flat, pct)) {
        return cost * pct / 100;
    }

    return cost;
}

// VERIFY: 9130a9d8 manual
// OFFSET: 0x007FF180
int32_t SpellRec::GetCastTime(const SpellRec* spell, int32_t isPet, int32_t isUnit, int32_t allowNegative) {
    if (!spell) {
        return 0;
    }

    SpellCastTimesRec* rec = g_spellCastTimesDB.GetRecord(spell->m_castingTimeIndex);

    if (!rec) {
        return 0;
    }

    int32_t castTime = rec->m_base + rec->m_perLevel * (static_cast<int32_t>(Spell_C_GetSpellLevel(spell, isPet, isUnit)) - spell->m_baseLevel);

    if (castTime < rec->m_minimum) {
        castTime = rec->m_minimum;
    }

    Spell_C_ModifySpellValue(spell, castTime, 10);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!(spell->m_attributes & 0x30) && !(spell->m_attributesExC & 0x20000000)) {
        if (isPet && unit) {
            const WOWGUID* guid = &unit->m_unit->m_unitFieldCharm;

            if (!*guid) {
                guid = &unit->m_unit->m_unitFieldSummon;
            }

            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*guid, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (unit && castTime > 0 && fnotequal(unit->m_unit->m_unitModCastSpeed, 1.0f)) {
            castTime = CMath::fint_(castTime * unit->m_unit->m_unitModCastSpeed);
        }
    }

    if (spell->m_attributes & 2) {
        castTime += 500;

        if (!isPet) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                CGBag_C* bag = &player->m_bag;

                if (bag) {
                    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID{ bag->GetItemGuid(17) }, TYPEMASK_ITEM, __FILE__, __LINE__));

                    if (item) {
                        ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, player->m_obj->m_guid, nullptr, nullptr, false);

                        if (stats && stats->m_delay) {
                            uint32_t speed = player->m_unit->m_unitFieldRangedattacktime;

                            if (speed) {
                                castTime = static_cast<int32_t>(static_cast<float>(speed) / static_cast<int32_t>(stats->m_delay) * castTime);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!allowNegative) {
        return castTime > 0 ? castTime : 0;
    }

    return castTime;
}

// VERIFY: 5d32a52a auto
// OFFSET: 0x007FF380
bool SpellRec::IsMeleeRange(const SpellRec* spell) {
    SpellRangeRec* rec = g_spellRangeDB.GetRecord(spell->m_rangeIndex);
    return rec && (rec->m_flags & 1);
}

// VERIFY: 62ade261 auto
// OFFSET: 0x007FF3C0
bool SpellRec::IsMinimumRange(const SpellRec* spell) {
    SpellRangeRec* rec = g_spellRangeDB.GetRecord(spell->m_rangeIndex);
    return rec && (rec->m_flags & 2);
}

// VERIFY: accadc29 manual
// OFFSET: 0x007FF400
void SpellRec::GetMinRange(const SpellRec* spell, float* range) {
    SpellRangeRec* rec = g_spellRangeDB.GetRecord(spell->m_rangeIndex);

    if (!rec) {
        return;
    }

    int32_t friendly = SpellRec::IsHelpfulOrHarmful(spell) == 1;

    if (rec->m_flags & 1) {
        *range = 0.0f;
    } else if (rec->m_flags << 1) {
        *range = 5.0f;
        *range += rec->m_rangeMin[friendly];
    } else {
        *range = rec->m_rangeMin[friendly];
    }
}

// VERIFY: 823383e2 manual
// OFFSET: 0x007FF480
void Spell_C_GetMinMaxRange(CGUnit_C* caster, const SpellRec* spell, float* minRange, float* maxRange, int32_t friendly, CGObject_C* target) {
    CGUnit_C* targetUnit = target && (target->m_obj->m_type & TYPEMASK_UNIT) ? static_cast<CGUnit_C*>(target) : nullptr;

    if (spell->m_attributes & 0x404) {
        *maxRange = 100.0f;
        return;
    }

    float bonus = 0.0f;
    SpellRangeRec* rec = g_spellRangeDB.GetRecord(spell->m_rangeIndex);

    if (rec) {
        if (rec->m_flags & 1) {
            *minRange = 0.0f;

            if (!targetUnit) {
                targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster->m_combat.m_attacking, TYPEMASK_UNIT, __FILE__, __LINE__));
            }

            float reach = targetUnit ? targetUnit->m_unit->m_unitFieldCombatreach : caster->m_unit->m_unitFieldCombatreach;
            bonus = reach + caster->m_unit->m_unitFieldCombatreach + 1.3333334f;

            if (bonus <= 5.0f) {
                bonus = 5.0f;
            }
        } else {
            if (rec->m_flags & 2) {
                float reach = targetUnit ? targetUnit->m_unit->m_unitFieldCombatreach : caster->m_unit->m_unitFieldCombatreach;
                *minRange = reach + caster->m_unit->m_unitFieldCombatreach + 1.3333334f;

                if (*minRange <= 5.0f) {
                    *minRange = 5.0f;
                }

                *minRange += rec->m_rangeMin[friendly];
            } else {
                *minRange = rec->m_rangeMin[friendly];
            }

            *maxRange = rec->m_rangeMax[friendly];

            if (target && (target->m_obj->m_type & 0x88)) {
                bonus = caster->m_unit->m_unitFieldCombatreach + (targetUnit ? targetUnit : caster)->m_unit->m_unitFieldCombatreach;

                if (*minRange != 0.0f && !(rec->m_flags & 2)) {
                    *minRange = bonus + *minRange;
                }
            }
        }

        if (targetUnit && (caster->m_movement->m_flags & 0x100D) && (targetUnit->m_movement->m_flags & 0x100D) && !caster->IsRunning() && !targetUnit->IsRunning() && ((rec->m_flags & 1) || (targetUnit->m_obj->m_type & TYPEMASK_PLAYER))) {
            bonus += 2.6666667f;
        }
    }

    if ((spell->m_attributes & 2) && (caster->m_obj->m_type & TYPEMASK_PLAYER)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(caster);
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID{ player->m_bag.GetItemGuid(17) }, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (item) {
            ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, caster->m_obj->m_guid, nullptr, nullptr, false);

            if (stats) {
                *maxRange = stats->m_rangedModRange * 0.01f * *maxRange;
            }
        }
    }

    int32_t flat;
    int32_t pct;

    if (Spell_C_GetSpellModifiers(spell, 5, flat, pct)) {
        *maxRange = (flat + *maxRange) * pct * 0.01f;
    }

    *maxRange = *maxRange + bonus;
}

// VERIFY: ac5e661b manual
// OFFSET: 0x007FF770
void Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, float* minPoints, float* maxPoints, int32_t spellLevel, int32_t isPet, int32_t isUnit, int32_t noModifiers) {
    *minPoints = 0.0f;
    *maxPoints = 0.0f;

    if (!spell) {
        return;
    }

    bool periodic;
    bool rounded;
    bool damage = GetEffectModifierType(spell->m_effect[effectIndex], &periodic, spell->m_effectAura[effectIndex], &rounded);
    int32_t level = spellLevel;

    if (!level) {
        level = Spell_C_GetSpellLevel(spell, isPet, isUnit);
    }

    if (spell->m_baseLevel > 0) {
        level -= spell->m_baseLevel;
    }

    if (level < 0) {
        level = 0;
    }

    CSoftFloat perLevel = *reinterpret_cast<const CSoftFloat*>(&spell->m_effectRealPointsPerLevel[effectIndex]);
    CSoftFloat minValue = CSoftFloat::FromInt(spell->m_effectBasePoints[effectIndex] + 1) + perLevel * CSoftFloat::FromInt(level);
    CSoftFloat maxValue = CSoftFloat::FromInt(spell->m_effectBasePoints[effectIndex] + spell->m_effectDieSides[effectIndex]) + perLevel * CSoftFloat::FromInt(level);

    if (!noModifiers) {
        Spell_C_ModifySpellValue(spell, minValue, 8);
        Spell_C_ModifySpellValue(spell, maxValue, 8);

        if (effectIndex == 0) {
            Spell_C_ModifySpellValue(spell, minValue, 3);
            Spell_C_ModifySpellValue(spell, maxValue, 3);
        } else if (effectIndex == 1) {
            Spell_C_ModifySpellValue(spell, minValue, 12);
            Spell_C_ModifySpellValue(spell, maxValue, 12);
        } else if (effectIndex == 2) {
            Spell_C_ModifySpellValue(spell, minValue, 23);
            Spell_C_ModifySpellValue(spell, maxValue, 23);
        }

        if (damage) {
            if (periodic) {
                Spell_C_ModifySpellValue(spell, minValue, 22);
                Spell_C_ModifySpellValue(spell, maxValue, 22);
            } else {
                Spell_C_ModifySpellValue(spell, minValue, 0);
                Spell_C_ModifySpellValue(spell, maxValue, 0);
            }
        }

        int32_t aura = spell->m_effectAura[effectIndex];

        if (aura == 10 || aura == 103 || aura == 183) {
            Spell_C_ModifySpellValue(spell, minValue, 2);
            Spell_C_ModifySpellValue(spell, maxValue, 2);
        }
    }

    minValue.m_bits += (minValue.m_bits & 0x7F800000) ? 0x3800000 : 0;
    minValue = CSoftFloat::Round(minValue);
    minValue.m_bits = (minValue.m_bits - 0x3800000) & ~static_cast<uint32_t>(static_cast<int32_t>((minValue.m_bits - 0x4000000) ^ minValue.m_bits) >> 31);
    maxValue.m_bits += (maxValue.m_bits & 0x7F800000) ? 0x3800000 : 0;
    maxValue = CSoftFloat::Round(maxValue);
    maxValue.m_bits = (maxValue.m_bits - 0x3800000) & ~static_cast<uint32_t>(static_cast<int32_t>((maxValue.m_bits - 0x4000000) ^ maxValue.m_bits) >> 31);

    if (rounded) {
        CSoftFloat low = CSoftFloat::Floor(minValue);
        *minPoints = *reinterpret_cast<float*>(&low);
        CSoftFloat high = CSoftFloat::Ceil(maxValue);
        *maxPoints = *reinterpret_cast<float*>(&high);
    } else {
        *minPoints = *reinterpret_cast<float*>(&minValue);
        *maxPoints = *reinterpret_cast<float*>(&maxValue);
    }
}

// VERIFY: 63ede24e manual
// OFFSET: 0x00800770
bool SpellRec::HasModifier(const SpellRec* spell, int32_t modifier) {
    int32_t flat;
    int32_t pct;
    return Spell_C_GetSpellModifiers(spell, modifier, flat, pct);
}

// VERIFY: 03db76ed manual
// OFFSET: 0x00800A70
int32_t Spell_C_GetDuration(const SpellRec* spell, int32_t isPet, int32_t isUnit, int32_t noModifiers, int32_t applyHaste) {
    if (!spell) {
        return 0;
    }

    SpellDurationRec* rec = g_spellDurationDB.GetRecord(spell->m_durationIndex);

    if (!rec) {
        return 0;
    }

    int32_t duration = rec->m_duration + rec->m_durationPerLevel * (static_cast<int32_t>(Spell_C_GetSpellLevel(spell, isPet, isUnit)) - spell->m_baseLevel);

    if (duration >= rec->m_maxDuration) {
        duration = rec->m_maxDuration;
    }

    if (!noModifiers) {
        Spell_C_ModifySpellValue(spell, duration, 1);
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !applyHaste) {
        return duration;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (spell->m_effectAuraPeriod[i]) {
            if (((spell->m_attributesExE & 0x2000) || player->AffectedByAura(316, spell)) && !(spell->m_attributesExC & 0x20000000) && player->m_unit->m_unitModCastSpeed >= 0.001f) {
                duration = CMath::fint_(duration * player->m_unit->m_unitModCastSpeed);
            }

            return duration;
        }
    }

    return duration;
}

// VERIFY: 885593c4 manual
// OFFSET: 0x00800D60
bool Spell_C_HasNoReagentCost(CGPlayer_C* player, const SpellRec* spell) {
    if ((player->m_unit->m_unitFieldFlags & 0x20) && (spell->m_attributesExE & 2)) {
        return true;
    }

    if (!spell->m_spellClassSet || spell->m_spellClassSet != s_spellClassSet) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (spell->m_spellClassMask[i] & player->m_player->m_playerNoReagentCost1[i]) {
            return true;
        }
    }

    return false;
}

// VERIFY: bfe60446 manual
// OFFSET: 0x008016C0
int32_t Spell_C_GetMinMaxPoints(const SpellRec* spell, uint32_t effectIndex, int32_t* minPoints, int32_t* maxPoints, int32_t a5, int32_t a6, int32_t a7, int32_t a8) {
    float low;
    float high;
    Spell_C_GetMinMaxPoints(spell, effectIndex, &low, &high, a5, a6, a7, a8);
    *minPoints = CMath::ftrunc(low);
    *maxPoints = CMath::ftrunc(high);
    return 0;
}

// VERIFY: 3cc995d6 auto
// OFFSET: 0x007FD630
int32_t Spell_C_GetTargetingSpell() {
    return s_targetingSpell ? s_targetingSpell->m_data.m_spellId : 0;
}

// VERIFY: 8d996461 auto
// OFFSET: 0x008090C0
bool Spell_C_GetItemCooldown(CGItem_C* item, uint32_t* duration, uint32_t* start, uint32_t* enable) {
    if (!item) {
        return false;
    }

    return s_spellHistory[0].GetSpellCooldown(item->GetEnchantmentSpell(0), item->m_obj->m_entryID, duration, start, enable);
}
