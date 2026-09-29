#include "UnitCombatLog_C.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <ctime>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"

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

// STATIC: 0x00CA1388
static uint32_t s_startTimeMs;

// STATIC: 0x00CA138C
static uint32_t s_startTime;

// STATIC: 0x00CA1390
static CombatLogEntry* s_currentEntry;

// STATIC: 0x00CA1394
static CombatLogEntry* s_pendingEntries;

// INLINED
inline CombatLogEntry::CombatLogEntry() {
    this->m_infoFlags = 0;
    this->Clear();
}

// INLINED
inline CombatLogEntry::~CombatLogEntry() {
    if (this->m_infoFlags & 0x2) {
        SMemFree(this->m_info.m_text, __FILE__, __LINE__, 0);
    }

    this->CancelPendingCallbacks();
}

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

// VERIFY: aa58db07 manual
// OFFSET: 0x0074D0D0
char* GUIDToHexString(WOWGUID guid, char* buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[18] = 0;

    uint64_t value = guid.m_guid;
    char* digit = &buffer[17];

    for (int32_t i = 0; i < 16; i++) {
        uint8_t nibble = value & 0xF;
        *digit-- = nibble >= 10 ? nibble + 'A' - 10 : nibble + '0';
        value >>= 4;
    }

    return buffer;
}

// VERIFY: b288c8ef auto
// OFFSET: 0x00756BD0
void UnitCombatClientInitialize() {
    ClientServices::SetMessageHandler(SMSG_ATTACK_START, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACK_STOP, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACKER_STATE_UPDATE, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_NOTINRANGE, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_BADFACING, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_DEADTARGET, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ATTACKSWING_CANT_ATTACK, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_ENVIRONMENTAL_DAMAGE_LOG, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_CLEAR_TARGET, Packet_Group_29, nullptr);
    ClientServices::SetMessageHandler(SMSG_COMBAT_EVENT_FAILED, Packet_Group_29, nullptr);
    s_entries.Clear();
    s_freeEntries.Clear();
    s_pendingEntries = nullptr;
    s_currentEntry = nullptr;
    s_filters.Clear();
    s_pendingData.Clear();
    s_freePendingData.Clear();
    s_invalidatedNames.Clear();
    s_startTime = static_cast<uint32_t>(time(nullptr));
    s_startTimeMs = FrameTime::s_curTimeMs;
    ClientServices::SetMessageHandler(SMSG_SPELL_PERIODIC_AURA_LOG, Packet_SMSG_PERIODICAURALOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_ENCHANTMENT_LOG, Packet_SMSG_ENCHANTMENTLOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_PARTY_KILL_LOG, Packet_SMSG_PARTYKILLLOG, nullptr);
    ClientServices::SetMessageHandler(SMSG_PROC_RESIST, Packet_SMSG_PROCRESIST, nullptr);
    ClientServices::SetMessageHandler(SMSG_DISPEL_FAILED, Packet_SMSG_DISPEL_FAILED, nullptr);
    ClientServices::SetMessageHandler(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, Packet_SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, nullptr);
    s_combatTextActiveUnit = WOWGUID{ 0 };
}

// VERIFY: 0526cc02 manual
// OFFSET: 0x00750270
void CombatLogEntry::CancelPendingCallbacks() {
    PendingCombatLogData* data = s_pendingData.Head();

    while (data) {
        if (data->m_entry != this) {
            data = s_pendingData.Next(data);
            continue;
        }

        switch (data->m_type) {
            case 1:
                g_nameDBCache.CancelCallback(data->m_guid.m_guid, &CombatLogEntry::PendingCombatLogResolved, data);
                break;
            case 2:
                g_petNameCache.CancelCallback(data->m_id, &CombatLogEntry::PendingCombatLogResolved, data);
                break;
            case 3:
                g_creatureDBCache.CancelCallback(data->m_id, &CombatLogEntry::PendingCombatLogResolved, data);
                break;
            case 4:
                g_gameObjectDBCache.CancelCallback(data->m_id, &CombatLogEntry::PendingCombatLogResolved, data);
                break;
            case 5:
                g_itemDBCache.CancelCallback(data->m_id, &CombatLogEntry::PendingCombatLogResolved, data);
                break;
        }

        data = s_pendingData.DeleteNode(data);
    }
}

// VERIFY: 65a90c44 manual
// OFFSET: 0x00750390
void RecycleCombatLogEntry(CombatLogEntry* entry) {
    if (s_pendingEntries == entry) {
        s_pendingEntries = s_entries.Next(entry);
    }

    if (s_currentEntry == entry) {
        s_currentEntry = s_entries.Next(entry);
    }

    entry->CancelPendingCallbacks();
    s_freeEntries.LinkToTail(entry);
}

// VERIFY: 40633cfb manual
// OFFSET: 0x0074F720
const char* CombatLogEntry::ResolveName(PendingCombatLogData* data, const char** realm, bool pending) {
    void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success) = pending ? &CombatLogEntry::PendingCombatLogResolved : nullptr;
    const char* name = nullptr;

    switch (data->m_type) {
        case 1: {
            NameCache* record = g_nameDBCache.GetRecord(data->m_guid.m_guid, data->m_guid, callback, data, false);

            if (record) {
                if (record->m_realmName[0] && realm) {
                    *realm = record->m_realmName;
                }

                name = record->m_name;
            }

            break;
        }
        case 2: {
            PetNameCache* record = g_petNameCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (!record) {
                break;
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit ? record->m_timestamp == unit->m_unit->m_unitFieldPetNameTimestamp : !pending) {
                name = record->m_name;
                break;
            }

            UnitCombatLogInvalidateName(record->m_name);
            g_petNameCache.Invalidate(data->m_id);
            g_petNameCache.GetRecord(data->m_id, data->m_guid, callback, data, false);
            break;
        }
        case 3: {
            CreatureStats_C* stats = g_creatureDBCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (stats) {
                name = stats->GetName(0);
            }

            break;
        }
        case 4: {
            GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (stats) {
                name = stats->GetName(0);
            }

            break;
        }
        case 5: {
            ItemStats_C* stats = g_itemDBCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (stats) {
                name = stats->GetName(0);
            }

            break;
        }
    }

    if (name) {
        return name;
    }

    const char* text = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);

    if (pending) {
        this->m_pendingMask |= data->m_mask;
        s_pendingData.LinkToTail(data);
    }

    return text;
}

// VERIFY: d93fe918 manual
// OFFSET: 0x0074FD40
const char* CombatLogEntry::GetObjectName(const WOWGUID& guid, uint32_t mask, const char** realm) {
    PendingCombatLogData* data = GetPendingCombatLogData(this, guid, mask);
    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

    if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid))) {
        data->m_type = 1;
    } else {
        uint32_t type = high & 0xF0F00000;

        if (type == 0xF0300000 || type == 0xF0500000) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!unit) {
                data->m_type = 3;
                data->m_id = static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF;
            } else if (unit->m_unit->m_unitFieldPetnumber) {
                data->m_type = 2;
                data->m_id = unit->m_unit->m_unitFieldPetnumber;
            } else {
                data->m_id = unit->m_obj->m_entryID;
                data->m_type = 3;
            }
        } else if (type == 0xF0400000) {
            data->m_id = static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF;
            data->m_type = 2;
        } else if (type == 0xF0100000) {
            data->m_id = static_cast<int32_t>(guid.m_guid >> 24) & 0xFFFFFFF;
            data->m_type = 4;
        } else {
            return FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
        }
    }

    return this->ResolveName(data, realm, true);
}

// VERIFY: 3635c9ab auto
// OFFSET: 0x0074FE90
void CombatLogEntry::PendingCombatLogResolved(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingCombatLogData* data = static_cast<PendingCombatLogData*>(param);
    CombatLogEntry* entry = data->m_entry;

    if (success) {
        const char* realm = nullptr;
        const char* name = entry->ResolveName(data, &realm, false);

        switch (data->m_mask) {
            case 1:
                entry->m_sourceName = name;
                entry->m_sourceRealm = realm;
                break;
            case 2:
                entry->m_destName = name;
                entry->m_destRealm = realm;
                break;
            case 4:
                entry->m_info.m_names[1] = name;
                break;
        }
    }

    entry->m_pendingMask &= ~data->m_mask;

    if (!entry->m_pendingMask) {
        CheckToSanitizeUnitLowGuid(&entry->m_sourceGuid);
        CheckToSanitizeUnitLowGuid(&entry->m_destGuid);

        if (s_pendingEntries == entry) {
            SendCombatLogEvents();
        }
    }

    s_freePendingData.LinkToTail(data);
}

// VERIFY: bfdde389 manual
// OFFSET: 0x0074DCB0
uint32_t GetCombatLogFlags(const WOWGUID& guid) {
    if (!guid.m_guid) {
        return 0x80000000;
    }

    bool inRaid = CGRaidInfo::m_numMembers != 0;
    bool inGroup = inRaid || CGPartyInfo::GetNumPartyMembers();
    WOWGUID owner = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    CGUnit_C* unit = nullptr;

    if (object) {
        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            unit = static_cast<CGUnit_C*>(object);

            if (unit->GetCharmerOrCreatorGuid()->m_guid) {
                owner = *unit->GetCharmerOrCreatorGuid();
            }
        } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
            owner = static_cast<CGGameObject_C*>(object)->m_gameObject->m_createdBy;
        }

        uint32_t ownerHigh = static_cast<uint32_t>(owner.m_guid >> 32);

        if (owner != guid && ((ownerHigh & 0xF0000000) || !((ownerHigh & 0xF07FFFFF) | static_cast<uint32_t>(owner.m_guid)))) {
            CGUnit_C* ownerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (ownerUnit && ownerUnit->GetCharmerOrCreatorGuid()->m_guid) {
                owner = *ownerUnit->GetCharmerOrCreatorGuid();
            }
        }
    }

    uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);
    bool isPlayer = !(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid));
    uint32_t type = high & 0xF0F00000;
    uint32_t flags;

    if (isPlayer || type == 0xF0300000 || type == 0xF0500000 || type == 0xF0400000) {
        if (owner == guid) {
            if (isPlayer) {
                flags = 0x400;
            } else {
                flags = type == 0xF0400000 ? 0x1000 : 0x800;
            }
        } else {
            const WOWGUID& controller = unit->m_unit->m_unitFieldCharmedby.m_guid ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldSummonedby;
            flags = controller.m_guid ? 0x1000 : 0x2000;
        }
    } else {
        flags = 0x4000;
    }

    uint32_t ownerHigh = static_cast<uint32_t>(owner.m_guid >> 32);

    if ((ownerHigh & 0xF0000000) || !((ownerHigh & 0xF07FFFFF) | static_cast<uint32_t>(owner.m_guid))) {
        flags |= 0x200;
    } else {
        flags |= 0x100;
    }

    if (owner == ClntObjMgrGetActivePlayer()) {
        flags |= 0x11;
    } else {
        bool hostile = false;

        if (unit) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                int32_t reaction = unit->UnitReaction(player);

                if (reaction >= 4) {
                    flags |= 0x10;
                } else if (reaction <= 1) {
                    flags |= 0x40;
                    hostile = true;
                }
            }
        }

        if (!hostile && inGroup) {
            if (CGPartyInfo::IsMember(owner)) {
                flags |= 0x12;
            } else if (inRaid && IsGuidInMyRaid(owner)) {
                flags |= 0x14;
            }
        }

        if (!(flags & 0xF)) {
            flags |= 0x8;
        }

        if (!(flags & 0xF0)) {
            flags |= 0x20;
        }
    }

    if (guid == CGGameUI::m_lockedTarget) {
        flags |= 0x10000;
    }

    if (guid == CGGameUI::unk_BD07D0) {
        flags |= 0x20000;
    }

    if ((flags & 0x400) && ((flags & 0x6) || (owner == ClntObjMgrGetActivePlayer() && inGroup))) {
        uint8_t assignments = inRaid ? CGRaidInfo::GetPartyAssignments(guid) : CGPartyInfo::GetPartyAssignments(guid);

        if (assignments & 0x2) {
            flags |= 0x40000;
        } else if (assignments & 0x4) {
            flags |= 0x80000;
        }
    }

    if (inGroup) {
        uint32_t index = GetRaidTargetIndexFromGuid(guid);

        if (index < 8) {
            return (0x100000 << index) | flags;
        }
    }

    return flags;
}

// VERIFY: e1ed1d33 auto
// OFFSET: 0x00750400
CombatLogEntry* GetCombatLogEntry(const WOWGUID& source, const WOWGUID& dest, int32_t event, const SpellRec* spell) {
    CombatLogEntry* entry = s_freeEntries.Head();

    if (!entry) {
        CombatLogEntry* oldest = s_entries.Head();

        if (oldest && static_cast<int32_t>(FrameTime::s_curTimeMs - 1000 * g_combatLogRetentionTimeCVar->m_intValue - oldest->m_timestamp) >= 0) {
            RecycleCombatLogEntry(oldest);
            entry = oldest;
        } else {
            entry = s_freeEntries.NewNode(2, 0, 0);
        }
    }

    entry->Clear();
    entry->m_event = event;

    if (source.m_guid && (!spell || !(spell->m_attributesExD & 0x2000))) {
        entry->m_sourceGuid = source;
        entry->m_sourceName = entry->GetObjectName(source, 1, &entry->m_sourceRealm);
        entry->m_sourceFlags = GetCombatLogFlags(source);
    }

    if (dest.m_guid) {
        entry->m_destGuid = dest;
        entry->m_destName = entry->GetObjectName(dest, 2, &entry->m_destRealm);
        entry->m_destFlags = GetCombatLogFlags(dest);
    }

    if (spell) {
        entry->m_spellId = spell->m_id;
        entry->m_spellName.Copy(spell->m_name);
    }

    return entry;
}

// VERIFY: c707d13e manual
// OFFSET: 0x0074F6C0
bool CombatLogEntry::MatchesFilter() {
    CombatLogFilter* filter = s_filters.Head();

    if (!filter) {
        return true;
    }

    for (; filter; filter = s_filters.Next(filter)) {
        if (filter->MatchesFilter(this->m_event, this->m_sourceGuid, this->m_sourceFlags, this->m_destGuid, this->m_destFlags, this->m_spellId, this->m_spellName.m_str)) {
            return true;
        }
    }

    return false;
}

// VERIFY: aecd56fc manual
// OFFSET: 0x0074F910
void SendCombatLogEvents() {
    char text[4096];
    bool logging = CGChat::IsChatLogging(1) != 0;
    lua_State* L = FrameScript_GetContext();
    lua_pushstring(L, g_scriptEvents[566]);
    CombatLogEntry* entry = s_pendingEntries;

    while (true) {
        int32_t count = entry->PushEvent(L);

        if (s_pendingEntries->MatchesFilter()) {
            FrameScript_SignalEvent(566, L, count + 1);
        }

        if (!FrameScript_GetEventListeners(g_scriptEvents[567])->Head()) {
            lua_settop(L, -1 - count);
        } else {
            lua_pushstring(L, g_scriptEvents[567]);
            lua_insert(L, -count - 1);
            FrameScript_SignalEvent(567, L, count + 1);
            lua_settop(L, -count - 2);
        }

        if (logging) {
            s_pendingEntries->PackEvent(text, sizeof(text));
            CGChat::AddCombatMessage(text);
        }

        entry = s_entries.Next(s_pendingEntries);

        if (!entry) {
            break;
        }

        s_pendingEntries = entry;

        if (entry->m_pendingMask) {
            lua_settop(L, -2);
            return;
        }
    }

    s_pendingEntries = nullptr;
    lua_settop(L, -2);
}

// VERIFY: fbd407d2 auto
// OFFSET: 0x0074FF20
void QueueCombatLogEntry(CombatLogEntry* entry, uint32_t timeOffset) {
    entry->m_timestamp = FrameTime::s_curTimeMs - timeOffset;
    s_entries.LinkToTail(entry);

    if (s_pendingEntries) {
        return;
    }

    s_pendingEntries = entry;

    if (!entry->m_pendingMask) {
        CheckToSanitizeUnitLowGuid(&entry->m_sourceGuid);
        CheckToSanitizeUnitLowGuid(&entry->m_destGuid);
        SendCombatLogEvents();
    }
}

// VERIFY: ebb52f1a auto
// OFFSET: 0x00754FD0
bool ShouldShowFeedback(const ATTACKROUNDINFO& info) {
    return !info.m_meleeSpellId || info.m_victimState == 1;
}

// VERIFY: b6cc2fa5 manual
// OFFSET: 0x007512B0
void UnitCombatLog(const ATTACKROUNDINFO& info) {
    if (info.m_hitInfo & 0x800000) {
        CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (attacker) {
            attacker->ModifyPredictedPower(1, info.m_rageGained);
        }
    }

    if (!ShouldShowFeedback(info)) {
        return;
    }

    uint32_t victimState = info.m_victimState;

    if ((victimState == 1 || victimState == 5) && info.m_damage) {
        int32_t damage = info.m_damage;
        int32_t resisted = 0;
        int32_t absorbed = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            resisted += info.m_resisted[i];
            absorbed += info.m_absorbed[i];
        }

        WOWGUID target = info.m_target;
        bool crushing = (info.m_hitInfo & 0x20000) != 0;
        bool glancing = (info.m_hitInfo >> 16) & 1;
        bool critical = (info.m_hitInfo & 0x200) != 0;
        int32_t blocked = info.m_blocked;
        uint32_t school = info.m_schoolMask[0];
        int32_t overkill = info.m_overkill;
        CombatLogEntry* entry = GetCombatLogEntry(info.m_attacker, target, 1, nullptr);
        entry->m_infoFlags |= 0x10;
        entry->m_info.m_values[2] = overkill;
        entry->m_info.m_values[4] = absorbed;
        entry->m_info.m_values[3] = school;
        entry->m_info.m_values[6] = blocked;
        entry->m_info.m_values[1] = damage;
        entry->m_info.m_values[5] = resisted;
        entry->m_infoBits = (entry->m_infoBits & ~0x7) | critical | ((glancing | (crushing << 1)) << 1);
        QueueCombatLogEntry(entry, 0);

        if (s_combatTextActiveUnit == target) {
            if (resisted > 0) {
                FrameScript_SignalEvent(484, "%s%d%d", s_combatTextTypes[10], damage, resisted);
            } else if (blocked) {
                SendToCombatText(11, damage, blocked);
            } else if (absorbed) {
                SendToCombatText(12, damage, absorbed);
            } else if (critical) {
                SendToCombatText(1, damage);
            } else {
                SendToCombatText(2, damage);
            }
        }

        return;
    }

    WOWGUID target = info.m_target;
    CombatLogEntry* entry;

    if (info.m_hitInfo & 0x10) {
        entry = GetCombatLogEntry(info.m_attacker, target, 2, nullptr);
        entry->SetMissInfo(1, 0);

        if (s_combatTextActiveUnit == target) {
            SendToCombatText(3);
        }
    } else if (info.m_hitInfo & 0x20) {
        int32_t absorbed = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            absorbed += info.m_absorbed[i];
        }

        entry = GetCombatLogEntry(info.m_attacker, target, 2, nullptr);
        entry->SetMissInfo(10, absorbed);

        if (s_combatTextActiveUnit == target) {
            SendToCombatText(12);
        }
    } else if (info.m_hitInfo & 0x80) {
        int32_t resisted = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            resisted += info.m_resisted[i];
        }

        entry = GetCombatLogEntry(info.m_attacker, target, 2, nullptr);
        entry->SetMissInfo(2, resisted);

        if (s_combatTextActiveUnit == target) {
            SendToCombatText(10);
        }
    } else {
        int32_t missType;
        int32_t textType;

        switch (victimState) {
            case 2:
                missType = 3;
                textType = 4;
                break;
            case 3:
                missType = 4;
                textType = 5;
                break;
            case 5:
                missType = 5;
                textType = 11;
                break;
            case 6:
                missType = 6;
                textType = 6;
                break;
            case 7:
                missType = 7;
                textType = 7;
                break;
            case 8:
                missType = 9;
                textType = 8;
                break;
            default:
                return;
        }

        entry = GetCombatLogEntry(info.m_attacker, target, 2, nullptr);
        entry->SetMissInfo(missType, info.m_blocked);

        if (s_combatTextActiveUnit == target) {
            SendToCombatText(textType);
        }
    }

    QueueCombatLogEntry(entry, 0);
}
