#include "UnitCombatLog_C.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/SpellRec.h"

// STATIC: 0x00ADB758
static const char* s_combatLogEventNames[50] = {
    "ENVIRONMENTAL_DAMAGE",
    "SWING_DAMAGE",
    "SWING_MISSED",
    "RANGE_DAMAGE",
    "RANGE_MISSED",
    "SPELL_CAST_START",
    "SPELL_CAST_SUCCESS",
    "SPELL_CAST_FAILED",
    "SPELL_MISSED",
    "SPELL_DAMAGE",
    "SPELL_HEAL",
    "SPELL_ENERGIZE",
    "SPELL_DRAIN",
    "SPELL_LEECH",
    "SPELL_INSTAKILL",
    "SPELL_SUMMON",
    "SPELL_CREATE",
    "SPELL_INTERRUPT",
    "SPELL_EXTRA_ATTACKS",
    "SPELL_DURABILITY_DAMAGE",
    "SPELL_DURABILITY_DAMAGE_ALL",
    "SPELL_AURA_APPLIED",
    "SPELL_AURA_APPLIED_DOSE",
    "SPELL_AURA_REMOVED_DOSE",
    "SPELL_AURA_REMOVED",
    "SPELL_AURA_REFRESH",
    "SPELL_DISPEL",
    "SPELL_STOLEN",
    "SPELL_AURA_BROKEN",
    "SPELL_AURA_BROKEN_SPELL",
    "DAMAGE_AURA_BROKEN",
    "ENCHANT_APPLIED",
    "ENCHANT_REMOVED",
    "SPELL_PERIODIC_MISSED",
    "SPELL_PERIODIC_DAMAGE",
    "SPELL_PERIODIC_HEAL",
    "SPELL_PERIODIC_ENERGIZE",
    "SPELL_PERIODIC_DRAIN",
    "SPELL_PERIODIC_LEECH",
    "SPELL_DISPEL_FAILED",
    "DAMAGE_SHIELD",
    "DAMAGE_SHIELD_MISSED",
    "DAMAGE_SPLIT",
    "PARTY_KILL",
    "UNIT_DIED",
    "UNIT_DESTROYED",
    "SPELL_RESURRECT",
    "SPELL_BUILDING_DAMAGE",
    "SPELL_BUILDING_HEAL",
    "UNIT_DISSIPATES"
};

// STATIC: 0x00ADB820
static const char* s_combatTextTypes[48] = {
    "INTERRUPT",
    "DAMAGE_CRIT",
    "DAMAGE",
    "MISS",
    "DODGE",
    "PARRY",
    "EVADE",
    "IMMUNE",
    "DEFLECT",
    "REFLECT",
    "RESIST",
    "BLOCK",
    "ABSORB",
    "SPELL_DAMAGE_CRIT",
    "SPELL_DAMAGE",
    "SPELL_MISS",
    "SPELL_DODGE",
    "SPELL_PARRY",
    "SPELL_EVADE",
    "SPELL_IMMUNE",
    "SPELL_DEFLECT",
    "SPELL_REFLECT",
    "SPELL_RESIST",
    "SPELL_BLOCK",
    "SPELL_ABSORB",
    "ENCHANTMENT_REMOVED",
    "ENCHANTMENT_ADDED",
    "PERIODIC_HEAL",
    "ENERGIZE",
    "PERIODIC_ENERGIZE",
    "SPELL_CAST",
    "SPELL_AURA_END",
    "SPELL_AURA_END_HARMFUL",
    "SPELL_AURA_START",
    "SPELL_AURA_START_HARMFUL",
    "SPELL_ACTIVE",
    "FACTION",
    "HEAL_CRIT",
    "HEAL",
    "DAMAGE_SHIELD",
    "SPELL_DISPELLED",
    "EXTRA_ATTACKS",
    "SPLIT_DAMAGE",
    "HONOR_GAINED",
    "PERIODIC_HEAL_ABSORB",
    "HEAL_CRIT_ABSORB",
    "HEAL_ABSORB",
    "ARENA_POINTS_GAINED"
};

// STATIC: 0x00ADB944
static TSList<InvalidatedName, TSGetLink<InvalidatedName> > s_invalidatedNames;

// STATIC: 0x00ADB950
static TSList<PendingCombatLogData, TSGetLink<PendingCombatLogData> > s_pendingData;

// STATIC: 0x00ADB95C
static TSList<PendingCombatLogData, TSGetLink<PendingCombatLogData> > s_freePendingData;

// STATIC: 0x00ADB968
static TSList<CombatLogFilter, TSGetLink<CombatLogFilter> > s_filters;

// STATIC: 0x00ADB974
static TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_entries;

// STATIC: 0x00ADB980
static TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_freeEntries;

// STATIC: 0x00CA1380
static WOWGUID s_combatTextActiveUnit;

// INLINED
inline InvalidatedName::InvalidatedName() : m_name(nullptr) {
}

// INLINED
inline InvalidatedName::~InvalidatedName() {
    SMemFree(this->m_name, __FILE__, __LINE__, 0);
}

// VERIFY: e107c6e5 manual
// OFFSET: 0x0074D1E0
void CombatLogEntry::SetFailReason(const char* reason) {
    this->m_infoFlags |= 0x2;
    this->m_info.m_text = SStrDupA(reason, __FILE__, __LINE__);
}

// VERIFY: 77d0900b auto
// OFFSET: 0x0074D210
void __stdcall CheckToSanitizeUnitLowGuid(WOWGUID* guid) {
    uint32_t type = static_cast<uint32_t>(guid->m_guid >> 32) & 0xF0F00000;

    if (type == 0xF0300000 || type == 0xF0500000) {
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(static_cast<int32_t>(guid->m_guid >> 24) & 0xFFFFFFF, *guid, nullptr, nullptr, false);

        if (stats && (stats->m_typeFlags & 0x4000)) {
            guid->m_guid &= 0xFFFFFFFFFF000000ull;
        }
    }
}

// VERIFY: 178bd755 auto
// OFFSET: 0x0074D430
int32_t IsCombatTextActiveUnit(WOWGUID guid) {
    return s_combatTextActiveUnit == guid;
}

// VERIFY: de1f3849 auto
// OFFSET: 0x0074D460
void SendToCombatText(int32_t type, const char* text, int32_t amount) {
    FrameScript_SignalEvent(484, "%s%s%d", s_combatTextTypes[type], text, amount);
}

// VERIFY: 0b7a7f5f auto
// OFFSET: 0x0074D490
void SendToCombatText(int32_t type, const char* text, int32_t amount, int32_t overflow) {
    FrameScript_SignalEvent(484, "%s%s%d%d", s_combatTextTypes[type], text, amount, overflow);
}

// VERIFY: 7e929dd0 auto
// OFFSET: 0x0074D4C0
void SendToCombatText(int32_t type, int32_t amount, int32_t overflow) {
    FrameScript_SignalEvent(484, "%s%d%d", s_combatTextTypes[type], amount, overflow);
}

// VERIFY: 22ab3dbb auto
// OFFSET: 0x0074D4F0
void SendToCombatText(int32_t type, int32_t amount) {
    FrameScript_SignalEvent(484, "%s%d", s_combatTextTypes[type], amount);
}

// VERIFY: b272b80c auto
// OFFSET: 0x0074D520
void SendToCombatText(int32_t type, const char* text) {
    FrameScript_SignalEvent(484, "%s%s", s_combatTextTypes[type], text);
}

// VERIFY: 69a4b870 auto
// OFFSET: 0x0074D550
void SendToCombatText(int32_t type) {
    FrameScript_SignalEvent(484, "%s", s_combatTextTypes[type]);
}

// VERIFY: be5d85cb auto
// OFFSET: 0x0074D700
void UnitCombatLogProcGo(const WOWGUID& casterGuid, const SpellRec* spell) {
    if (s_combatTextActiveUnit == casterGuid && (spell->m_attributesExF & 0x40)) {
        FrameScript_SignalEvent(484, "%s%s", s_combatTextTypes[30], spell->m_name);
    }
}

// VERIFY: ab3c05fe manual
// OFFSET: 0x0074D920
void CombatLogEntry::Clear() {
    this->m_event = -1;
    this->m_pendingMask = 0;
    this->m_sourceGuid = WOWGUID();
    this->m_sourceName = nullptr;
    this->m_sourceRealm = nullptr;
    this->m_sourceFlags = 0x80000000;
    this->m_destGuid = WOWGUID();
    this->m_destName = nullptr;
    this->m_destRealm = nullptr;
    this->m_destFlags = 0x80000000;
    this->m_spellId = 0;
    this->m_spellName.Copy(static_cast<const char*>(nullptr));

    if (this->m_infoFlags & 0x2) {
        SMemFree(this->m_info.m_text, __FILE__, __LINE__, 0);
    }

    this->m_infoFlags = 0;
}

// VERIFY: 10cdb977 manual
// OFFSET: 0x0074D980
void CombatLogEntry::SetMissInfo(int32_t missType, int32_t amount) {
    this->m_infoFlags |= 0x4;
    this->m_info.m_values[0] = missType;

    switch (missType) {
        case 2:
            this->m_infoFlags |= 0x4000;
            this->m_info.m_values[5] = amount;
            break;
        case 5:
            this->m_infoFlags |= 0x8000;
            this->m_info.m_values[6] = amount;
            break;
        case 10:
            this->m_infoFlags |= 0x2000;
            this->m_info.m_values[4] = amount;
            break;
    }
}

// VERIFY: 4abe76e4 manual
// OFFSET: 0x0074E050
bool CombatLogFilter::SetFilterData(const char* events, const WOWGUID& sourceGuid, uint32_t sourceFlags, const WOWGUID& destGuid, uint32_t destFlags, int32_t spellId, const char* spellName) {
    if (events) {
        if (this->m_events.m_count) {
            memset(this->m_events.m_bits.m_data, 0, sizeof(uint32_t) * this->m_events.m_bits.m_count);
        }

        char token[128];

        do {
            SStrTokenize(&events, token, sizeof(token), " ,", nullptr);

            if (!token[0]) {
                break;
            }

            for (uint32_t i = 0; i < 50; i++) {
                if (!SStrCmpI(token, s_combatLogEventNames[i], STORM_MAX_STR)) {
                    this->m_events.m_bits.m_data[i >> 5] |= 1 << (i & 0x1F);
                    break;
                }
            }
        } while (token[0] && *events);
    } else if (this->m_events.m_count) {
        memset(this->m_events.m_bits.m_data, 0xFF, sizeof(uint32_t) * this->m_events.m_bits.m_count);
    }

    if (sourceGuid) {
        this->m_sourceGuid = sourceGuid;
        this->m_sourceFlags = 0;
    } else {
        this->m_sourceGuid = WOWGUID();
        this->m_sourceFlags = sourceFlags;
    }

    if (destGuid) {
        this->m_destGuid = destGuid;
        this->m_destFlags = 0;
    } else {
        this->m_destGuid = WOWGUID();
        this->m_destFlags = destFlags;
    }

    if (spellId) {
        this->m_spellId = spellId;
        this->m_spellName.Copy(static_cast<const char*>(nullptr));
        return true;
    }

    this->m_spellId = 0;
    this->m_spellName.Copy(spellName);
    return true;
}

// VERIFY: 23038301 manual
// OFFSET: 0x0074F350
PendingCombatLogData* GetPendingCombatLogData(CombatLogEntry* entry, const WOWGUID& guid, uint32_t mask) {
    PendingCombatLogData* data = s_freePendingData.Head();

    if (!data) {
        data = s_freePendingData.NewNode(2, 0, 0);
    }

    data->m_entry = entry;
    data->m_guid = guid;
    data->m_id = 0;
    data->m_type = 0;
    data->m_mask = mask;
    return data;
}

// VERIFY: 106263c5 manual
// OFFSET: 0x0074F400
void UnitCombatLogInvalidateName(const char* name) {
    InvalidatedName* node = s_invalidatedNames.NewNode(2, 0, 0);
    node->m_name = SStrDupA(name, __FILE__, __LINE__);

    for (CombatLogEntry* entry = s_entries.Head(); entry; entry = s_entries.Next(entry)) {
        if (entry->m_sourceName == name) {
            entry->m_sourceName = node->m_name;
        }

        if (entry->m_destName == name) {
            entry->m_destName = node->m_name;
        }
    }
}
