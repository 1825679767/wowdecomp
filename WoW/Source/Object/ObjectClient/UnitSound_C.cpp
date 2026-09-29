#include "UnitSound_C.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00ADB5D8
static const uint32_t s_unitSoundChance[14] = { 70, 100, 60, 100, 100, 40, 100, 100, 100, 100, 100, 100, 100, 0 };

// STATIC: 0x00CA12D0
static CVar* s_cvFootstepSounds;

// STATIC: 0x00CA12D4
static uint32_t s_lastStandSoundTime;

// STATIC: 0x00CA12D8
static TSFixedArray<DEATTHUDSOUNDINFO> s_deathThudSounds[5];

// INLINED
inline DEATTHUDSOUNDINFO::DEATTHUDSOUNDINFO() : m_soundEntryId(0), m_soundEntryIdWater(0) {
}

// VERIFY: 1674f405 manual
// OFFSET: 0x00747760
static void UnitSoundLoadDeathThuds() {
    int32_t maxSoundId = g_terrainTypeSoundsDB.m_maxID;

    for (int32_t i = 0; i < 5; i++) {
        s_deathThudSounds[i].SetCount(maxSoundId + 1);
    }

    for (int32_t i = g_deathThudLookupsDB.m_numRecords; i; ) {
        i--;
        DeathThudLookupsRec* rec = i >= 0 && i < g_deathThudLookupsDB.m_numRecords ? &g_deathThudLookupsDB.m_records[i] : nullptr;

        if (static_cast<uint32_t>(rec->m_sizeClass) < 5 && rec->m_terrainTypeSoundID <= maxSoundId) {
            s_deathThudSounds[rec->m_sizeClass].m_data[rec->m_terrainTypeSoundID].m_soundEntryId = rec->m_soundEntryID;
            s_deathThudSounds[rec->m_sizeClass].m_data[rec->m_terrainTypeSoundID].m_soundEntryIdWater = rec->m_soundEntryIdwater;
        }
    }
}

// VERIFY: d5edf73b manual
// OFFSET: 0x00746540
void CGUnit_C::SetImpactKitEffect(bool offhand, const ATTACKROUNDINFO* info, const C3Vector* position) {
    CGUnit_C* attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info->m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__));
    const CGUnitVirtualItem* weapon = attacker ? attacker->GetAttackingWeapon((info->m_hitInfo >> 2) & 1, false) : nullptr;
    const CGUnitVirtualItem* parry = this->GetParryingItem(offhand);

    if (parry) {
        int32_t self = s_activeMover == info->m_attacker || s_activeMover == info->m_target;
        SI2::PlayParrySound(weapon, parry, info->m_hitInfo & 0x200, position, self);
    }
}

// VERIFY: 37583731 manual
// OFFSET: 0x00747860
void UnitSoundInitialize() {
    UnitSoundLoadDeathThuds();

    s_cvFootstepSounds = CVar::Register("FootstepSounds", nullptr, 0, "1", nullptr, 5, false, nullptr, false);
    s_lastStandSoundTime = FrameTime::s_curTimeMs;

    for (int32_t i = 0; i < g_soundEntriesDB.m_numRecords; i++) {
        SoundEntriesRec* rec = i >= 0 && i < g_soundEntriesDB.m_numRecords ? &g_soundEntriesDB.m_records[i] : nullptr;

        if (!SStrCmp(rec->m_name, "SpiritWolf (DONOTRENAME)", STORM_MAX_STR)) {
            CGUnit_C::s_dismountSoundKitId = rec->m_id;
            return;
        }
    }
}

// VERIFY: e952682c manual
// OFFSET: 0x00747810
void UnitSoundShutdown() {
    for (int32_t i = 0; i < 5; i++) {
        s_deathThudSounds[i].Clear();
    }
}

// VERIFY: 7e015e11 auto
// OFFSET: 0x007461E0
int32_t GetSoundID(const CreatureSoundDataRec* soundData, uint32_t soundType) {
    if (!soundData) {
        return 0;
    }

    switch (soundType) {
        case 0:
            return soundData->m_soundExertionID;
        case 1:
            return soundData->m_soundExertionCriticalID;
        case 2:
        case 13:
            return soundData->m_soundInjuryID;
        case 3:
            return soundData->m_soundInjuryCriticalID;
        case 4:
            return soundData->m_soundStunID;
        case 5:
            return soundData->m_soundStandID;
        case 7:
            return soundData->m_soundWingFlapID;
        case 8:
            return soundData->m_soundAlertID;
        case 9:
            return soundData->m_soundInjuryCrushingBlowID;
        case 10:
            return soundData->m_soundWingGlideID;
        case 11:
            return soundData->m_soundJumpStartID;
        case 12:
            return soundData->m_soundJumpEndID;
        default:
            return 0;
    }
}

// VERIFY: dab78e5a auto
// OFFSET: 0x007464A0
int32_t CheckUnitSoundTimer(uint32_t soundType) {
    if (soundType == 5) {
        if (static_cast<int32_t>(FrameTime::s_curTimeMs - s_lastStandSoundTime - 10000) < 0) {
            return 0;
        }

        s_lastStandSoundTime = FrameTime::s_curTimeMs;
    }

    return 1;
}

// VERIFY: fcb45083 auto
// OFFSET: 0x007462E0
void CGUnit_C::PlayFidgetSound(uint32_t index) {
    CreatureSoundDataRec* soundData = this->GetSoundData();

    if (!soundData || index >= 5) {
        return;
    }

    int32_t soundKitId = soundData->m_soundFidget[index];

    if (!soundKitId) {
        return;
    }

    C3Vector position = this->GetPosition();
    position.z += 2.0f;
    SI2::PlaySoundKit(soundKitId, &position, nullptr, nullptr, 0, nullptr, 1, 0);
}

// VERIFY: 339e65d9 auto
// OFFSET: 0x00746610
void CGUnit_C::PlayDeathThud() {
    C3Vector position = this->GetPosition();
    int32_t liquidType;
    float liquidLevel;
    int32_t liquidFlags;
    int32_t inLiquid = World::QueryObjectLiquid(this->m_worldObject, &liquidType, &liquidLevel, &liquidFlags);

    if (inLiquid && liquidLevel - position.z > 2.0f) {
        return;
    }

    uint32_t size = this->GetUnitSize();

    if (size >= 5) {
        return;
    }

    TerrainTypeRec* terrainType = g_terrainTypeDB.GetRecord(this->m_terrainTypeId);

    if (!terrainType || terrainType->m_soundID >= static_cast<int32_t>(s_deathThudSounds[size].m_count)) {
        return;
    }

    DEATTHUDSOUNDINFO* info = &s_deathThudSounds[size].m_data[terrainType->m_soundID];
    int32_t soundKitId = inLiquid ? info->m_soundEntryIdWater : info->m_soundEntryId;

    if (!soundKitId) {
        int32_t defaults[5] = { 907, 912, 917, 922, 927 };
        soundKitId = defaults[size];
        SysMsgPrintf(STATUS_WARNING, 2, "INVALID DEATH THUD SOUND: DEFAULTING TO ID %d", soundKitId);
    }

    SI2::PlaySoundKit(soundKitId, &position, nullptr, nullptr, 0, nullptr, 1, 0);
}

// VERIFY: 8792f0f0 auto
// OFFSET: 0x007471A0
void CGUnit_C::HandlePlayStandSound(uint32_t eventId, int32_t a3) {
    switch (eventId) {
        case 0x31444624:
            this->PlayFidgetSound(0);
            break;
        case 0x32444624:
            this->PlayFidgetSound(1);
            break;
        case 0x33444624:
            this->PlayFidgetSound(2);
            break;
        case 0x34444624:
            this->PlayFidgetSound(3);
            break;
        case 0x35444624:
            this->PlayFidgetSound(4);
            break;
        case 0x36444624:
        case 0x37444624:
        case 0x38444624:
        case 0x39444624:
            this->GetSoundData();
            break;
        case 0x58444624:
            this->PlayStandSound();
            break;
    }
}

// VERIFY: 73f75c40 manual
// OFFSET: 0x00747240
void CGUnit_C::HandleFootfallAnimEvent(const C3Vector& position) {
    if ((this->m_unit->m_unitFieldBytes1[2] & 0x2) || (this->m_movementData.m_flags & MOVEMENTFLAG_HOVER)) {
        return;
    }

    if (this->m_vehiclePassenger && this->m_vehiclePassenger->m_state) {
        return;
    }

    if ((this->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(this)->m_player->m_playerFlags & 0x10)) {
        return;
    }

    this->PlayFoleySound();

    CreatureSoundDataRec* soundData = this->GetSoundData();

    if (soundData && soundData->m_soundFootstepID && s_cvFootstepSounds && s_cvFootstepSounds->m_intValue) {
        SI2::PlayFootstepSound(soundData->m_soundFootstepID, &position, this->m_terrainTypeId, this->IsSplashing(position), this->GetUnitSize() >= 3, this);
    }
}

// VERIFY: 63312a72 manual
// OFFSET: 0x00747310
void CGUnit_C::PlayUnitSound(uint32_t soundType, int32_t force) {
    if (!force && static_cast<uint32_t>((static_cast<uint64_t>(CRandom::uint32_(g_rndSeed)) * 101) >> 32) > s_unitSoundChance[soundType]) {
        return;
    }

    if (soundType == 5) {
        if (static_cast<int32_t>(FrameTime::s_curTimeMs - s_lastStandSoundTime - 10000) < 0) {
            return;
        }

        s_lastStandSoundTime = FrameTime::s_curTimeMs;
    }

    int32_t soundKitId = GetSoundID(this->GetSoundData(), soundType);

    if (!soundKitId) {
        return;
    }

    C3Vector position = this->GetPosition();

    if (CGUnit_C::s_activeMover == this->m_obj->m_guid || this->IsActivePlayer()) {
        static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

        int32_t listenerAtCharacter = s_listenerAtCharacter->m_intValue;
        SE2SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_group = this->GetUnitSoundKitIndex(soundType);

        if (listenerAtCharacter) {
            properties.m_volumeScale = 0.64999998f;
        }

        properties.m_priority = 110;
        SI2::PlaySoundKit(soundKitId, listenerAtCharacter ? nullptr : &position, nullptr, &properties, 0, nullptr, 1, 0);
    } else {
        SE2SoundKitProperties properties;
        properties.ResetToDefaults();

        if (soundType == 7) {
            properties.m_maxAudibleBehavior = 0;
        }

        properties.m_group = this->GetUnitSoundKitIndex(soundType);
        SI2::PlaySoundKit(soundKitId, &position, nullptr, &properties, 0, nullptr, 1, 0);
    }
}
