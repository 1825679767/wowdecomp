#include <cstdint>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Ui/PetInfo.h"

int32_t Script_GetAccountExpansionLevel(lua_State* L);
int32_t Script_UnitIsInMyGuild(lua_State* L);
int32_t Script_UnitIsPartyLeader(lua_State* L);
int32_t Script_UnitIsRaidOfficer(lua_State* L);
int32_t Script_UnitInParty(lua_State* L);
int32_t Script_UnitPlayerOrPetInParty(lua_State* L);
int32_t Script_UnitInRaid(lua_State* L);
int32_t Script_UnitPlayerOrPetInRaid(lua_State* L);
int32_t Script_UnitIsAFK(lua_State* L);
int32_t Script_UnitIsDND(lua_State* L);
int32_t Script_UnitIsPVPSanctuary(lua_State* L);
int32_t Script_UnitIsPVPFreeForAll(lua_State* L);
int32_t Script_UnitFactionGroup(lua_State* L);
int32_t Script_UnitReaction(lua_State* L);
int32_t Script_UnitIsEnemy(lua_State* L);
int32_t Script_UnitCanAssist(lua_State* L);
int32_t Script_UnitCanAttack(lua_State* L);
int32_t Script_PlayerCanTeleport(lua_State* L);
int32_t Script_UnitSelectionColor(lua_State* L);
int32_t Script_UnitGUID(lua_State* L);
int32_t Script_UnitName(lua_State* L);
int32_t Script_UnitPVPName(lua_State* L);
int32_t Script_UnitMana(lua_State* L);
int32_t Script_UnitManaMax(lua_State* L);
int32_t Script_UnitPower(lua_State* L);
int32_t Script_UnitPowerMax(lua_State* L);
int32_t Script_UnitPowerType(lua_State* L);
int32_t Script_UnitClassBase(lua_State* L);
int32_t Script_UnitResistance(lua_State* L);
int32_t Script_UnitStat(lua_State* L);
int32_t Script_UnitAttackBothHands(lua_State* L);
int32_t Script_UnitDamage(lua_State* L);
int32_t Script_UnitRangedDamage(lua_State* L);
int32_t Script_UnitRangedAttack(lua_State* L);
int32_t Script_UnitAttackSpeed(lua_State* L);
int32_t Script_UnitAttackPower(lua_State* L);
int32_t Script_UnitRangedAttackPower(lua_State* L);
int32_t Script_UnitDefense(lua_State* L);
int32_t Script_UnitArmor(lua_State* L);
int32_t Script_UnitIsTapped(lua_State* L);
int32_t Script_UnitIsTappedByPlayer(lua_State* L);
int32_t Script_UnitIsTappedByAllThreatList(lua_State* L);
int32_t Script_UnitIsTrivial(lua_State* L);
int32_t Script_UnitHasRelicSlot(lua_State* L);
int32_t Script_SetPortraitTexture(lua_State* L);
int32_t Script_HasFullControl(lua_State* L);
int32_t Script_GetComboPoints(lua_State* L);
int32_t Script_IsInGuild(lua_State* L);
int32_t Script_IsGuildLeader(lua_State* L);
int32_t Script_IsArenaTeamCaptain(lua_State* L);
int32_t Script_IsInArenaTeam(lua_State* L);
int32_t Script_IsResting(lua_State* L);
int32_t Script_GetCombatRating(lua_State* L);
int32_t Script_GetCombatRatingBonus(lua_State* L);
int32_t Script_GetMaxCombatRatingBonus(lua_State* L);
int32_t Script_GetDodgeChance(lua_State* L);
int32_t Script_GetBlockChance(lua_State* L);
int32_t Script_GetShieldBlock(lua_State* L);
int32_t Script_GetParryChance(lua_State* L);
int32_t Script_GetCritChanceFromAgility(lua_State* L);
int32_t Script_GetCritChance(lua_State* L);
int32_t Script_GetRangedCritChance(lua_State* L);
int32_t Script_GetSpellCritChance(lua_State* L);
int32_t Script_GetSpellBonusHealing(lua_State* L);
int32_t Script_GetPetSpellBonusDamage(lua_State* L);
int32_t Script_GetSpellPenetration(lua_State* L);
int32_t Script_GetArmorPenetration(lua_State* L);
int32_t Script_GetAttackPowerForStat(lua_State* L);
int32_t Script_UnitCreatureType(lua_State* L);
int32_t Script_UnitCreatureFamily(lua_State* L);
int32_t Script_GetResSicknessDuration(lua_State* L);
int32_t Script_GetPVPSessionStats(lua_State* L);
int32_t Script_GetPVPYesterdayStats(lua_State* L);
int32_t Script_GetPVPLifetimeStats(lua_State* L);
int32_t Script_UnitPVPRank(lua_State* L);
int32_t Script_GetPVPRankInfo(lua_State* L);
int32_t Script_UnitCastingInfo(lua_State* L);
int32_t Script_UnitChannelInfo(lua_State* L);
int32_t Script_IsLoggedIn(lua_State* L);
int32_t Script_IsFlyableArea(lua_State* L);
int32_t Script_IsIndoors(lua_State* L);
int32_t Script_IsOutdoors(lua_State* L);
int32_t Script_IsOutOfBounds(lua_State* L);
int32_t Script_IsFalling(lua_State* L);
int32_t Script_IsSwimming(lua_State* L);
int32_t Script_IsFlying(lua_State* L);
int32_t Script_IsMounted(lua_State* L);
int32_t Script_IsStealthed(lua_State* L);
int32_t Script_UnitIsSameServer(lua_State* L);
int32_t Script_GetUnitHealthModifier(lua_State* L);
int32_t Script_GetUnitMaxHealthModifier(lua_State* L);
int32_t Script_GetUnitPowerModifier(lua_State* L);
int32_t Script_GetUnitHealthRegenRateFromSpirit(lua_State* L);
int32_t Script_GetUnitManaRegenRateFromSpirit(lua_State* L);
int32_t Script_GetManaRegen(lua_State* L);
int32_t Script_GetPowerRegen(lua_State* L);
int32_t Script_GetRuneCount(lua_State* L);
int32_t Script_ReportPlayerIsPVPAFK(lua_State* L);
int32_t Script_PlayerIsPVPInactive(lua_State* L);
int32_t Script_GetExpertise(lua_State* L);
int32_t Script_GetExpertisePercent(lua_State* L);
int32_t Script_UnitInRange(lua_State* L);
int32_t Script_GetUnitSpeed(lua_State* L);
int32_t Script_GetUnitPitch(lua_State* L);
int32_t Script_UnitInVehicle(lua_State* L);
int32_t Script_UnitUsingVehicle(lua_State* L);
int32_t Script_UnitControllingVehicle(lua_State* L);
int32_t Script_UnitInVehicleControlSeat(lua_State* L);
int32_t Script_UnitTargetsVehicleInRaidUI(lua_State* L);
int32_t Script_UnitVehicleSkin(lua_State* L);
int32_t Script_UnitVehicleSeatCount(lua_State* L);
int32_t Script_UnitVehicleSeatInfo(lua_State* L);
int32_t Script_UnitSwitchToVehicleSeat(lua_State* L);
int32_t Script_CanSwitchVehicleSeat(lua_State* L);
int32_t Script_GetVehicleUIIndicator(lua_State* L);
int32_t Script_GetVehicleUIIndicatorSeat(lua_State* L);
int32_t Script_UnitDetailedThreatSituation(lua_State* L);
int32_t Script_UnitIsControlling(lua_State* L);
int32_t Script_EjectPassengerFromSeat(lua_State* L);
int32_t Script_CanEjectPassengerFromSeat(lua_State* L);
int32_t Script_RespondInstanceLock(lua_State* L);
int32_t Script_GetPlayerFacing(lua_State* L);
int32_t Script_GetPlayerInfoByGUID(lua_State* L);
int32_t Script_GetItemStats(lua_State* L);
int32_t Script_GetItemStatDelta(lua_State* L);
int32_t Script_IsXPUserDisabled(lua_State* L);

// STATIC: 0x009FE7EC
static const int32_t s_unitSexValues[3] = { 2, 3, 1 };

// STATIC: 0x00AD21BC
static const char* s_unitClassificationTokens[7] = { "normal", "elite", "rareelite", "worldboss", "rare", "trivial", nullptr };

// VERIFY: e1b7cfaf manual
// OFFSET: 0x0060FD40
static int32_t Script_UnitRace(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitRace(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    ChrRacesRec* rec;
    const char* name;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        rec = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());
        name = CGUnit_C::GetDisplayRaceNameFromRecord(rec, ClientServices::GetCharacterSex(), nullptr);
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            rec = g_chrRacesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[0]);
            name = CGUnit_C::GetDisplayRaceName(unit, nullptr);
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), nullptr, nullptr, false);

            if (!nameCache) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            rec = g_chrRacesDB.GetRecord(nameCache->m_race);
            name = CGUnit_C::GetDisplayRaceName(nullptr, nameCache);
        }
    }

    if (rec && name) {
        lua_pushstring(L, name);
        lua_pushstring(L, rec->m_clientFileString);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// VERIFY: 5774e509 manual
// OFFSET: 0x0060FEC0
static int32_t Script_UnitClass(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitClass(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    ChrClassesRec* rec;
    const char* name;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        rec = g_chrClassesDB.GetRecord(ClientServices::GetCharacterClass());
        name = CGUnit_C::GetDisplayClassNameFromRecord(rec, ClientServices::GetCharacterSex(), nullptr);
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1]);
            name = CGUnit_C::GetDisplayClassName(unit, nullptr);
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), nullptr, nullptr, false);

            if (!nameCache) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            rec = g_chrClassesDB.GetRecord(nameCache->m_class);
            name = CGUnit_C::GetDisplayClassName(nullptr, nameCache);
        }
    }

    if (rec && name) {
        lua_pushstring(L, name);
        lua_pushstring(L, rec->m_filename);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// VERIFY: 33f0f651 manual
// OFFSET: 0x0060C350
static int32_t Script_UnitIsVisible(lua_State* L) {
    WOWGUID guid;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false) && ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 0c112a91 manual
// OFFSET: 0x0060C4B0
static int32_t Script_UnitIsPlayer(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;

    if (ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__) || CGGameUI::IsPartyMember(unitGuid)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 1260bb78 manual
// OFFSET: 0x0060C6F0
static int32_t Script_UnitIsCorpse(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);

    if (ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 3c39c5f7 manual
// OFFSET: 0x0060CBA0
static int32_t Script_UnitPlayerControlled(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldFlags & 0x8)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: acc75bfe manual
// OFFSET: 0x0060D7D0
static int32_t Script_UnitIsCharmed(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && unit->m_unit->m_unitFieldCharmedby.m_guid) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: b79b1a6b manual
// OFFSET: 0x0060D860
static int32_t Script_UnitIsPossessed(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldFlags & 0x1000000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: a95a418f manual
// OFFSET: 0x0060D970
static int32_t Script_UnitClassification(lua_State* L) {
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)))) {
        lua_pushstring(L, s_unitClassificationTokens[unit->GetCreatureClassification()]);
        return 1;
    }

    lua_pushstring(L, s_unitClassificationTokens[0]);
    return 1;
}

// VERIFY: dcb1f0ea manual
// OFFSET: 0x0060EB60
static int32_t Script_UnitHealth(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHealth(\"unit\")");
        return 0;
    }

    int32_t health = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!(unit->m_unit->m_unitDynamicFlags & 0x20) || CGGameUI::IsRaidMemberOrPet(unitGuid)) {
            health = unit->GetPredictedHealth();
        } else {
            health = 0;
        }
    } else if (CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid)) {
        health = state->m_health;
    } else if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
        health = petState->m_health;
    }

    lua_pushnumber(L, health);
    return 1;
}

// VERIFY: 9147ef39 manual
// OFFSET: 0x0060F350
static int32_t Script_UnitOnTaxi(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitOnTaxi(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_unit->m_unitFieldFlags & 0x100000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 38246c4a manual
// OFFSET: 0x0060F3D0
static int32_t Script_UnitIsFeignDeath(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsFeignDeath(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit) {
        WOWGUID guid = unit->m_obj->m_guid;

        if (CGGameUI::IsRaidMemberOrPet(guid) && (unit->m_unit->m_unitDynamicFlags & 0x20)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 98944afe manual
// OFFSET: 0x0060F480
static int32_t Script_UnitIsDead(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDead(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_unit->m_unitFieldHealth <= 0) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x20) {
            guid = unit->m_obj->m_guid;

            if (!CGGameUI::IsRaidMemberOrPet(guid)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid);

        if (state && (state->m_flags & 0x4)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 293e1200 manual
// OFFSET: 0x0060F580
static int32_t Script_UnitIsGhost(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsGhost(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && (static_cast<CGPlayer_C*>(unit)->m_player->m_playerFlags & 0x10)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid);

        if (state && (state->m_flags & 0x8)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 0a346c3b manual
// OFFSET: 0x0060F680
static int32_t Script_UnitIsDeadOrGhost(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDeadOrGhost(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->IsDeadOrGhost()) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        if (unit->m_unit->m_unitDynamicFlags & 0x20) {
            guid = unit->m_obj->m_guid;

            if (!CGGameUI::IsRaidMemberOrPet(guid)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid);

        if (state && (state->m_flags & 0xC)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 86a32d5f manual
// OFFSET: 0x0060F790
static int32_t Script_UnitIsConnected(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsConnected(\"unit\")");
        return 0;
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;

    if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__)) {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid);
        bool connected = state ? (state->m_flags & 0x1) != 0 : CGPartyMemberStateRepository::FindPetState(unitGuid) != nullptr;

        if (!connected) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: dceb316e manual
// OFFSET: 0x0060F860
static int32_t Script_UnitAffectingCombat(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAffectingCombat(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_unit->m_unitFieldFlags & 0x80000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 8a5d087b manual
// OFFSET: 0x0060F8E0
static int32_t Script_UnitSex(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitSex(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    int32_t sex;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        sex = s_unitSexValues[ClientServices::GetCharacterSex()];
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            sex = s_unitSexValues[unit->m_unit->m_unitFieldBytes0[2]];
        } else {
            NameCache* nameCache = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), nullptr, nullptr, false);
            sex = nameCache ? s_unitSexValues[nameCache->m_sex] : 1;
        }
    }

    lua_pushnumber(L, sex);
    return 1;
}

// VERIFY: 5c0c6a81 manual
// OFFSET: 0x0060FC40
static int32_t Script_GetHonorCurrency(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    lua_pushnumber(L, player ? player->m_player->m_playerFieldHonorCurrency : 0.0);
    lua_pushnumber(L, 75000.0);
    return 2;
}

// VERIFY: 08a8189c manual
// OFFSET: 0x0060FCC0
static int32_t Script_GetArenaCurrency(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    lua_pushnumber(L, player ? player->m_player->m_playerFieldArenaCurrency : 0.0);
    lua_pushnumber(L, 10000.0);
    return 2;
}

// VERIFY: 57ab2b88 manual
// OFFSET: 0x0060A510
static int32_t Script_FillLocalizedClassList(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: FillLocalizedClassList(classTable[, isFemale])");
    }

    int32_t female = StringToBOOL(L, 2, 0) != 0;
    uint32_t count = g_chrClassesDB.m_numRecords;
    lua_settop(L, 1);

    for (int32_t i = 0; static_cast<uint32_t>(i) < count; i++) {
        ChrClassesRec* rec = (i < 0 || i >= g_chrClassesDB.m_numRecords) ? nullptr : &g_chrClassesDB.m_records[i];

        if (rec) {
            lua_pushstring(L, rec->m_filename);
            lua_pushstring(L, CGUnit_C::GetDisplayClassNameFromRecord(rec, female, nullptr));
            lua_settable(L, -3);
        }
    }

    return 1;
}

// VERIFY: b12edcb3 manual
// OFFSET: 0x0060C2A0
static int32_t Script_UnitExists(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if ((object && object->CanBeTargetted()) || CGGameUI::IsRaidMemberOrPet(unitGuid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 77367e09 manual
// OFFSET: 0x0060EC60
static int32_t Script_UnitHealthMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHealthMax(\"unit\")");
        return 0;
    }

    int32_t maxHealth = 0;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        maxHealth = unit->m_unit->m_unitFieldMaxhealth;
    } else if (CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid)) {
        maxHealth = state->m_maxHealth;
    } else if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
        maxHealth = petState->m_maxHealth;
    }

    lua_pushnumber(L, maxHealth);
    return 1;
}

// VERIFY: f3c90ef7 manual
// OFFSET: 0x0060EA60
static int32_t Script_UnitXP(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitXP(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, static_cast<CGPlayer_C*>(unit)->GetCurrentXP());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 2a64d447 manual
// OFFSET: 0x0060EAE0
static int32_t Script_UnitXPMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitXPMax(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        lua_pushnumber(L, static_cast<CGPlayer_C*>(unit)->GetNextLevelXP());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: ff612f66 manual
// OFFSET: 0x0060FBA0
static int32_t Script_GetMoney(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, player->GetCoinage());
    return 1;
}

// VERIFY: 177658fd auto
// OFFSET: 0x006081F0
static int32_t Script_GetTime(lua_State* L) {
    lua_pushnumber(L, OsGetAsyncTimeMs() * 0.001);

    return 1;
}

// VERIFY: 4e5ce7ae auto
// OFFSET: 0x00608230
static int32_t Script_GetGameTime(lua_State* L) {
    lua_pushnumber(L, g_clientGameTime.m_hour);
    lua_pushnumber(L, g_clientGameTime.m_minute);

    return 2;
}

// VERIFY: e5f40d18 auto
// OFFSET: 0x00608270
static int32_t Script_ConsoleExec(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ConsoleExec(\"console_command\")");
        return 0;
    }

    ConsoleCommandExecute(lua_tolstring(L, 1, nullptr), 0);

    return 0;
}

// VERIFY: 20774f42 auto
// OFFSET: 0x00510BB0
static int32_t Script_ReadFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// VERIFY: 972d4262 auto
// OFFSET: 0x00510BB0
static int32_t Script_DeleteFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// VERIFY: f25bf2fd auto
// OFFSET: 0x00510BB0
static int32_t Script_AppendToFile(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// VERIFY: 507fc501 manual
// OFFSET: 0x0060E740
static int32_t Script_UnitName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitName(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        lua_pushstring(L, ClientServices::GetCharacterName());
        lua_pushnil(L);
        return 2;
    }

    const char* realm = nullptr;
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID unitGuid = guid;

    if (!guid || guid.m_guid == 0xFFFFFFFFFFFFFFFEULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    const char* name = nullptr;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        uint32_t type = object->m_obj->m_type;

        if (type & TYPEMASK_UNIT) {
            name = static_cast<CGUnit_C*>(object)->GetUnitName(&realm, 1);
        } else if (type & TYPEMASK_GAMEOBJECT) {
            name = static_cast<CGGameObject_C*>(object)->CGGameObject_C::GetObjectName();
        } else if (type & TYPEMASK_ITEM) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(object->m_obj->m_entryID, none, nullptr, nullptr, false);

            if (stats) {
                name = stats->GetName(0);
            }
        } else if (type & TYPEMASK_CORPSE) {
            WOWGUID owner = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldOwner;
            NameCache* record = g_nameDBCache.GetRecord(owner.m_guid, owner, UnitNameQueryCallback, nullptr, true);

            if (record) {
                name = record->m_name;

                if (record->m_realmName[0]) {
                    realm = record->m_realmName;
                }
            }
        }
    } else if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
        name = reinterpret_cast<const char*>(petState->unk_0008);
    } else if ((unitGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
        PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((unitGuid.m_guid >> 24) & 0xFFFFFFF), unitGuid, UnitNameQueryCallback, nullptr, true);

        if (record) {
            name = record->m_name;
        }
    } else {
        NameCache* record = g_nameDBCache.GetRecord(unitGuid.m_guid, unitGuid, UnitNameQueryCallback, nullptr, true);

        if (record) {
            name = record->m_name;
            realm = record->m_realmName;
        }
    }

    if (!name) {
        name = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
    }

    lua_pushstring(L, name);
    lua_pushstring(L, realm);
    return 2;
}

// VERIFY: f9ddec63 manual
// OFFSET: 0x0060ED40
static int32_t Script_UnitPower(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPower(\"unit\"[, type])");
    }

    uint32_t powerType = 7;

    if (lua_isnumber(L, 2)) {
        powerType = static_cast<uint32_t>(lua_tointeger(L, 2));

        if (powerType >= 8) {
            lua_pushnumber(L, 0.0);
            return 1;
        }
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    double power = 0.0;

    if (unit) {
        if (powerType == 7) {
            powerType = unit->m_unit->m_unitFieldBytes0[3];
        }

        if ((!(unit->m_unit->m_unitDynamicFlags & 0x20) || CGGameUI::IsRaidMemberOrPet(unit->m_obj->m_guid)) && (powerType == unit->m_unit->m_unitFieldBytes0[3] || unit->IsActivePlayer())) {
            power = static_cast<uint32_t>(unit->GetPredictedPower(powerType)) / Spell_C_GetPowerDisplayMod(powerType);
        }
    } else if (CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid)) {
        if (powerType == 7 || powerType == state->m_powerType) {
            power = state->m_power / Spell_C_GetPowerDisplayMod(state->m_powerType);
        }
    } else if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
        if (powerType == 7 || powerType == petState->m_powerType) {
            power = petState->m_power / Spell_C_GetPowerDisplayMod(petState->m_powerType);
        }
    }

    lua_pushnumber(L, power);
    return 1;
}

// VERIFY: db11e0a8 manual
// OFFSET: 0x0060EF40
static int32_t Script_UnitPowerMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPowerMax(\"unit\"[, type])");
    }

    uint32_t powerType = 7;

    if (lua_isnumber(L, 2)) {
        powerType = static_cast<uint32_t>(lua_tointeger(L, 2));

        if (powerType >= 8) {
            lua_pushnumber(L, 0.0);
            return 1;
        }
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    double power = 0.0;

    if (unit) {
        if (powerType == 7) {
            powerType = unit->m_unit->m_unitFieldBytes0[3];
        }

        power = unit->m_unit->m_unitFieldMaxpower[powerType] / Spell_C_GetPowerDisplayMod(powerType);
    } else if (CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid)) {
        if (powerType == 7 || powerType == state->m_powerType) {
            power = state->m_maxPower / Spell_C_GetPowerDisplayMod(state->m_powerType);
        }
    } else if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
        if (powerType == 7 || powerType == petState->m_powerType) {
            power = petState->m_maxPower / Spell_C_GetPowerDisplayMod(petState->m_powerType);
        }
    }

    lua_pushnumber(L, power);
    return 1;
}

// VERIFY: 2aad3ea0 manual
// OFFSET: 0x0060F100
static int32_t Script_UnitPowerType(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPowerType(\"unit\"[, index])");
    }

    uint32_t index = 0;

    if (lua_isnumber(L, 2)) {
        index = static_cast<uint32_t>(lua_tointeger(L, 2) - 1);
    }

    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && unit->m_vehicle && unit->m_vehicle->m_vehicleRec && (unit->m_vehicle->m_vehicleRec->m_flags & 0x100000) && index < 3) {
        PowerDisplayRec* display = g_powerDisplayDB.GetRecord(unit->m_vehicle->m_vehicleRec->m_powerDisplayID[index]);

        if (display && display->m_actualType == unit->m_unit->m_unitFieldBytes0[3]) {
            lua_pushnumber(L, display->m_actualType);
            lua_pushstring(L, display->m_globalStringBaseTag);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_red) * 0.0039215689f);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_green) * 0.0039215689f);
            lua_pushnumber(L, static_cast<uint8_t>(display->m_blue) * 0.0039215689f);
            return 5;
        }
    }

    if (!index) {
        if (unit) {
            lua_pushnumber(L, unit->m_unit->m_unitFieldBytes0[3]);
            lua_pushstring(L, GetPowerString(unit->m_unit->m_unitFieldBytes0[3]));
            return 2;
        }

        if (CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(unitGuid)) {
            lua_pushnumber(L, state->m_powerType);
            lua_pushstring(L, GetPowerString(state->m_powerType));
            return 2;
        }

        if (CGPartyPetState* petState = CGPartyMemberStateRepository::FindPetState(unitGuid)) {
            lua_pushnumber(L, petState->m_powerType);
            lua_pushstring(L, GetPowerString(petState->m_powerType));
            return 2;
        }
    }

    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    return 2;
}

// VERIFY: cc9043bb manual
// OFFSET: 0x0060D0A0
static int32_t Script_UnitFactionGroup(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitFactionGroup(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    int32_t factionTemplate = 0;

    if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        ChrRacesRec* race = g_chrRacesDB.GetRecord(ClientServices::GetCharacterRace());

        if (!race) {
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }

        factionTemplate = race->m_factionID;
    } else {
        WOWGUID guid = WOWGUID();
        Script_GetGUIDFromToken(token, guid, false);
        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
            factionTemplate = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldFactiontemplate;
        } else if (object && (object->m_obj->m_type & TYPEMASK_CORPSE)) {
            factionTemplate = static_cast<CGCorpse_C*>(object)->GetFactionTemplate();
        } else {
            if (!guid) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            WOWGUID none = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, none, nullptr, nullptr, false);
            ChrRacesRec* race = record ? g_chrRacesDB.GetRecord(record->m_race) : nullptr;

            if (!race) {
                lua_pushnil(L);
                lua_pushnil(L);
                return 2;
            }

            factionTemplate = race->m_factionID;
        }
    }

    FactionTemplateRec* faction = factionTemplate ? g_factionTemplateDB.GetRecord(factionTemplate) : nullptr;

    if (faction) {
        for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
            FactionGroupRec* group = i >= 0 && i < g_factionGroupDB.m_numRecords ? &g_factionGroupDB.m_records[i] : nullptr;

            if (((1 << group->m_maskID) & faction->m_factionGroup) && *group->m_name) {
                lua_pushstring(L, group->m_internalName);
                lua_pushstring(L, group->m_name);
                return 2;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// VERIFY: 6fdf06f2 auto
// OFFSET: 0x0060A450
static int32_t Script_IsLoggedIn(lua_State* L) {
    if (CGGameUI::m_loggingIn) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: af10718e auto
// OFFSET: 0x00608580
static int32_t Script_CanSwitchVehicleSeat(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::GetActive();

    if (inputControl) {
        lua_pushboolean(L, inputControl->IsVehicleSeatFlagSet(0x4000000));
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// VERIFY: 775130e4 manual
// OFFSET: 0x00614140
static int32_t Script_IsXPUserDisabled(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushboolean(L, (player->m_player->m_playerFlags >> 25) & 1);
    return 1;
}

// VERIFY: 40c866c6 manual
// OFFSET: 0x0060DB20
static int32_t Script_IsInGuild(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b1cb14d5 manual
// OFFSET: 0x0060DF30
static int32_t Script_GetDodgeChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerDodgePercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: a0d343c0 manual
// OFFSET: 0x0060DF90
static int32_t Script_GetBlockChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerBlockPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: ea63cefb manual
// OFFSET: 0x0060E070
static int32_t Script_GetParryChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerParryPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 053898a7 manual
// OFFSET: 0x0060E0D0
static int32_t Script_GetCritChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerCritPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 3461ae9d manual
// OFFSET: 0x0060E230
static int32_t Script_GetRangedCritChance(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, (player->m_player->m_playerRangedCritPercentage));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: a05d6e3f manual
// OFFSET: 0x0060E3B0
static int32_t Script_GetSpellBonusHealing(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerFieldModHealingDonePos));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 1d179dfa manual
// OFFSET: 0x0060E410
static int32_t Script_GetPetSpellBonusDamage(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerPetSpellPower));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 8eff02c1 manual
// OFFSET: 0x00612300
static int32_t Script_IsIndoors(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !player->IsOutdoors()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 35e28049 manual
// OFFSET: 0x00612360
static int32_t Script_IsOutdoors(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->IsOutdoors()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 283ecf8d manual
// OFFSET: 0x006124A0
static int32_t Script_IsSwimming(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_clientFlags & 0x200000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: e6af303d manual
// OFFSET: 0x00612610
static int32_t Script_IsStealthed(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_unit->m_unitFieldBytes1[2] & 2)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: d4ae0785 manual
// OFFSET: 0x0060DB80
static int32_t Script_IsGuildLeader(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_player->m_playerGuildid && !player->m_player->m_playerGuildrank) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 3c7dfaa2 manual
// OFFSET: 0x0060DD40
static int32_t Script_IsResting(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 447b42ee manual
// OFFSET: 0x0060E470
static int32_t Script_GetSpellPenetration(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, -static_cast<int32_t>(player->m_player->m_playerFieldModTargetResistance));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 4aef4c33 manual
// OFFSET: 0x00611600
static int32_t Script_HasFullControl(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_combatModeFlags & 1) && player->m_unit->m_unitFieldHealth > 0 && CGGameUI::m_hasControl) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 4bbfb5e1 manual
// OFFSET: 0x00611C40
static int32_t Script_UnitPVPRank(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPVPRank(\"unit\")");
    }

    GetUnitFromName(lua_tolstring(L, 1, nullptr));
    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: 2bbe9059 manual
// OFFSET: 0x006123C0
static int32_t Script_IsOutOfBounds(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x4000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b246aef0 manual
// OFFSET: 0x00612430
static int32_t Script_IsFalling(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t flags = player->m_movement->m_flags;

        if ((flags & 0x1000) && !(flags & 0x800)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: daa462e7 manual
// OFFSET: 0x006125A0
static int32_t Script_IsMounted(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_mountDisplayId <= 0 || (player->m_clientFlags & 0x10000000)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: 8334ae2c auto
// OFFSET: 0x0060C920
static int32_t Script_UnitInParty(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID member = guid;

    if (CGPartyInfo::IsMember(member)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2c33b711 manual
// OFFSET: 0x0060D8F0
static int32_t Script_PlayerCanTeleport(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t flags = player->m_unit->m_unitFieldFlags;

    if ((flags & 0x40000) || (flags & 0x800000) || (flags & 0x400000)) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    }

    return 1;
}

// VERIFY: 734bc0d8 manual
// OFFSET: 0x0060DBF0
static int32_t Script_IsInArenaTeam(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (uint32_t i = 0; i < 3; i++) {
            if (player->m_player->m_playerFieldArenaTeamInfo11[i * 7]) {
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

// VERIFY: 5eae70e6 manual
// OFFSET: 0x0060DFF0
static int32_t Script_GetShieldBlock(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerShieldBlock);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: f607142d manual
// OFFSET: 0x0060E290
static int32_t Script_GetSpellCritChance(lua_State* L) {
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (school > 6) {
        luaL_error(L, "Usage: GetSpellCritChance(school)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    lua_pushnumber(L, player ? player->m_player->m_playerSpellCritPercentage1[school] : 0.0f);
    return 1;
}

// VERIFY: a0133d89 auto
// OFFSET: 0x00611130
static int32_t Script_UnitIsTapped(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTapped(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 4)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: a55a9031 auto
// OFFSET: 0x006111B0
static int32_t Script_UnitIsTappedByPlayer(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTappedByPlayer(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: d33dc156 auto
// OFFSET: 0x00611230
static int32_t Script_UnitIsTappedByAllThreatList(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTappedByAllThreatList(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_unit->m_unitDynamicFlags & 0x80)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 10c64bd9 auto
// OFFSET: 0x006112B0
static int32_t Script_UnitIsTrivial(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsTrivial(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && CGPlayer_C::UnitIsTrivial(unit)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 335afb42 manual
// OFFSET: 0x0060C770
static int32_t Script_UnitIsPartyLeader(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((player && (player->m_player->m_playerFlags & 1)) || CGPartyInfo::m_leader == guid) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 66591369 manual
// OFFSET: 0x0060CF20
static int32_t Script_UnitIsPVPSanctuary(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid;
    CGUnit_C* unit;

    if (Script_GetGUIDFromToken(token, guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (unit->m_unit->m_unitFieldBytes2[1] & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 57bc1e86 auto
// OFFSET: 0x0060D330
static int32_t Script_UnitIsEnemy(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsEnemy(\"unit\", \"otherUnit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    CGUnit_C* other = GetUnitFromName(lua_tolstring(L, 2, nullptr));

    if (unit && other && unit->UnitReaction(other) <= 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 23480833 auto
// OFFSET: 0x0060D690
static int32_t Script_UnitCanAssist(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanAssist(\"unit\", \"otherUnit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    CGUnit_C* other = GetUnitFromName(lua_tolstring(L, 2, nullptr));

    if (unit && other && unit->CanAssist(other, false)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 5d6fa4bf auto
// OFFSET: 0x0060D730
static int32_t Script_UnitCanAttack(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanAttack(\"unit\", \"otherUnit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    CGUnit_C* other = GetUnitFromName(lua_tolstring(L, 2, nullptr));

    if (unit && other && unit->CanAttack(other)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2a1a0dff auto
// OFFSET: 0x00611780
static int32_t Script_UnitCreatureType(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCreatureType(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    int32_t type;
    CreatureTypeRec* rec;

    if (unit && (type = unit->GetCreatureType()) && (rec = g_creatureTypeDB.GetRecord(type))) {
        lua_pushstring(L, rec->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 592e2b17 auto
// OFFSET: 0x00611820
static int32_t Script_UnitCreatureFamily(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCreatureFamily(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    uint32_t family;
    CreatureFamilyRec* rec;

    if (unit && (family = unit->GetCreatureFamily()) && (rec = g_creatureFamilyDB.GetRecord(family))) {
        lua_pushstring(L, rec->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 3baa631d manual
// OFFSET: 0x00612500
static int32_t Script_IsFlying(lua_State* L) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state == 3) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID{ unit->GetTransportGUID() }, TYPEMASK_UNIT, __FILE__, __LINE__));
    }

    if (unit && (unit->m_movement->m_flags & 0x2000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: e52973fe manual
// OFFSET: 0x00612870
static int32_t Script_GetUnitMaxHealthModifier(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitMaxHealthModifier(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->m_unit->m_unitFieldMaxhealthmodifier + 1.0);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 7c1ca0c5 manual
// OFFSET: 0x00612CB0
static int32_t Script_GetExpertisePercent(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerExpertise) * 0.25);
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerOffhandExpertise) * 0.25);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: 1e5f92f8 manual
// OFFSET: 0x00613290
static int32_t Script_GetUnitSpeed(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitSpeed(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    lua_pushnumber(L, unit ? unit->m_movement->m_currentSpeed : 0.0f);
    return 1;
}

// VERIFY: 94bb89b8 manual
// OFFSET: 0x00613330
static int32_t Script_GetUnitPitch(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetUnitPitch(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    lua_pushnumber(L, unit ? unit->GetPitch() : 0.0f);
    return 1;
}

// VERIFY: 5733704d manual
// OFFSET: 0x00613570
static int32_t Script_UnitControllingVehicle(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    int32_t controlling = 0;
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state == 3 && unit->GetTransportGUID() == unit->m_unit->m_unitFieldCharm.m_guid) {
            controlling = 1;
        }
    }

    lua_pushboolean(L, controlling);
    return 1;
}

// VERIFY: 5df6566b manual
// OFFSET: 0x00613830
static int32_t Script_UnitVehicleSeatCount(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid;
    CGUnit_C* unit;
    CGUnit_C* root;

    if (Script_GetGUIDFromToken(token, guid, false) && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__))) && (root = unit->FindRootVehicle(nullptr)) && root->m_vehicle && root->m_vehicle->m_vehicleRec) {
        lua_pushnumber(L, root->m_vehicle->GetVirtualSeatCount());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 8fd44e43 manual
// OFFSET: 0x00613C90
static int32_t Script_UnitIsControlling(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid;

    if (Script_GetGUIDFromToken(token, guid, false)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            CGUnitData* data = unit->m_unit;

            if (data->m_unitFieldCharm ? data->m_unitFieldCharm : data->m_unitFieldSummon) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 3718aa54 manual
// OFFSET: 0x006082C0
static int32_t Script_GetMaxCombatRatingBonus(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMaxCombatRatingBonus(ratingIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 25) {
        luaL_error(L, "ratingIndex is in the range 1 .. %d", 25);
        return 0;
    }

    lua_pushnumber(L, index < 14 || index > 16 ? -1.0 : 0x1.080000Bp+5);
    return 1;
}

// VERIFY: 18494640 auto
// OFFSET: 0x0060D280
static int32_t Script_UnitReaction(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitReaction(\"unit\", \"otherUnit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    CGUnit_C* other = GetUnitFromName(lua_tolstring(L, 2, nullptr));

    if (unit && other) {
        lua_pushnumber(L, unit->UnitReaction(other) + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 0b0e6f1d manual
// OFFSET: 0x00612A90
static int32_t Script_GetManaRegen(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_unit->m_unitFieldBytes0[3]) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    lua_pushnumber(L, player->GetPowerRegen(0, false) + 0.001f);
    lua_pushnumber(L, player->GetPowerRegen(0, true) + 0.001f);
    return 2;
}

// VERIFY: e854f343 manual
// OFFSET: 0x00612B40
static int32_t Script_GetPowerRegen(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint8_t powerType = player->m_unit->m_unitFieldBytes0[3];
        lua_pushnumber(L, player->GetPowerRegen(powerType, false) + 0.001f);
        lua_pushnumber(L, player->GetPowerRegen(powerType, true) + 0.001f);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: 7aa85aa4 manual
// OFFSET: 0x0060DDB0
static int32_t Script_GetCombatRating(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCombatRating(ratingIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 25) {
        luaL_error(L, "ratingIndex is in the range 1 .. %d", 25);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerFieldCombatRating1[index]));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: ccddb858 manual
// OFFSET: 0x00612BF0
static int32_t Script_GetExpertise(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerExpertise);
        lua_pushnumber(L, player->m_player->m_playerOffhandExpertise);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: 4960208f manual
// OFFSET: 0x00611A20
static int32_t Script_GetPVPSessionStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerFieldKills[0]);
        lua_pushnumber(L, player->m_player->m_playerFieldTodayContribution);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: ccc92036 manual
// OFFSET: 0x00611AD0
static int32_t Script_GetPVPYesterdayStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerFieldKills[1]);
        lua_pushnumber(L, player->m_player->m_playerFieldYesterdayContribution);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: 22d944ac manual
// OFFSET: 0x00611B80
static int32_t Script_GetPVPLifetimeStats(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, player->m_player->m_playerFieldLifetimeHonorbaleKills);
        uint8_t rank = player->m_player->m_playerFieldBytes[3];
        lua_pushnumber(L, rank < 5 ? 0.0 : rank);
        return 2;
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// VERIFY: fe0bce8d manual
// OFFSET: 0x0060DC70
static int32_t Script_IsArenaTeamCaptain(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsArenaTeamCaptain(index)");
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t* info = player && index < 3 ? &player->m_player->m_playerFieldArenaTeamInfo11[index * 7] : nullptr;

    if (!info || info[2]) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// VERIFY: f7665c53 manual
// OFFSET: 0x0060CD50
static int32_t Script_UnitIsDND(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsDND(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID stateGuid = guid;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t dnd;

    if (player) {
        dnd = player->m_player->m_playerFlags >> 2;
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(stateGuid);

        if (!state) {
            lua_pushnil(L);
            return 1;
        }

        dnd = static_cast<uint8_t>(state->m_flags) >> 7;
    }

    if (dnd & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 124502f9 manual
// OFFSET: 0x006133D0
static int32_t Script_UnitInVehicle(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitInVehicle(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID stateGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!unit->m_vehiclePassenger || unit->m_vehiclePassenger->m_state != 3) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(stateGuid);

        if (!state || !(state->m_flags & 0x200)) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: 2d3eaa21 manual
// OFFSET: 0x006134A0
static int32_t Script_UnitUsingVehicle(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitUsingVehicle(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID stateGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!unit->m_vehiclePassenger || !unit->m_vehiclePassenger->m_state) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(stateGuid);

        if (!state || !(state->m_flags & 0x200)) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: c02f7d5f manual
// OFFSET: 0x00611330
static int32_t Script_UnitHasRelicSlot(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitHasRelicSlot(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    ChrClassesRec* rec;

    if (unit && (unit->m_obj->m_type & TYPEMASK_PLAYER) && (rec = g_chrClassesDB.GetRecord(unit->m_unit->m_unitFieldBytes0[1])) && (rec->m_flags & 8)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 90024a7e manual
// OFFSET: 0x0060E9A0
static int32_t Script_UnitPVPName(lua_State* L) {
    char buffer[1024];

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitPVPName(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        lua_pushstring(L, unit->GetUnitChosenTitle(buffer, sizeof(buffer), 1, nullptr));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: c276bfad manual
// OFFSET: 0x0060CFB0
static int32_t Script_UnitIsPVPFreeForAll(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsPVPFreeForAll(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID stateGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    uint32_t ffa;

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        ffa = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldBytes2[1] >> 2;
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(stateGuid);

        if (!state) {
            lua_pushnil(L);
            return 1;
        }

        ffa = static_cast<uint8_t>(state->m_flags) >> 4;
    }

    if (ffa & 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 199a309f manual
// OFFSET: 0x00610EC0
static int32_t Script_UnitArmor(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitArmor(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    int32_t total = 0;
    int32_t effective = 0;
    int32_t positive = 0;
    int32_t negative = 0;
    int32_t base = 0;

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        unit->GetResistanceAndBuffs(GetPhysicalDamageClassID(), &base, &total, &effective, &positive, &negative);
    }

    lua_pushnumber(L, base);
    lua_pushnumber(L, effective);
    lua_pushnumber(L, total);
    lua_pushnumber(L, positive);
    lua_pushnumber(L, negative);
    return 5;
}

// VERIFY: 15fb0fea manual
// OFFSET: 0x00613D20
static int32_t Script_CanEjectPassengerFromSeat(lua_State* L) {
    int32_t canEject = 0;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanEjectPassengerFromSeat(seatIndex)");
    }

    int32_t seatIndex = static_cast<int32_t>(lua_tonumber(L, 1));
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGUnit_C* root;
    const CVehicle_C* vehicle;
    uint8_t seat;
    CGUnit_C* passenger;
    VehicleSeatRec* seatRec;

    if (mover && (root = mover->FindRootVehicle(nullptr)) && root->m_vehicle && root->m_vehicle->m_vehicleRec && root->m_vehicle->GetVehicleAndSeatFromVirtualSeat(seatIndex - 1, vehicle, seat) && (passenger = const_cast<CVehicle_C*>(vehicle)->FindPassengerUnit(seat)) && passenger->FindRootVehicle(mover) == mover && (seatRec = vehicle->GetVehicleSeatRec(seat)) && (seatRec->m_flagsB & 0x20)) {
        canEject = 1;
    }

    lua_pushboolean(L, canEject);
    return 1;
}

// VERIFY: cef8c43a manual
// OFFSET: 0x00610300
static int32_t Script_UnitStat(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UnitStat(\"unit\", statIndex)");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    uint32_t stat = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (stat > 4) {
        luaL_error(L, "Invalid stat index in UnitStat");
        return 0;
    }

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        lua_pushnumber(L, unit->m_unit->m_unitFieldStat[stat]);
        lua_pushnumber(L, unit->GetEffectiveStat(stat));
        lua_pushnumber(L, unit->m_unit->m_unitFieldPosstat[stat]);
        lua_pushnumber(L, unit->m_unit->m_unitFieldNegstat[stat]);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 4;
}

// VERIFY: dbe049a8 manual
// OFFSET: 0x006101A0
static int32_t Script_UnitResistance(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: UnitResistance(\"unit\", resistanceIndex)");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (school > 6) {
        luaL_error(L, "Invalid resistance index in UnitResistance");
        return 0;
    }

    int32_t total = 0;
    int32_t positive = 0;
    int32_t negative = 0;
    int32_t base = 0;
    int32_t effective;

    if (unit && (unit->m_obj->m_type & TYPEMASK_UNIT)) {
        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            unit->GetResistanceAndBuffs(school, &base, &total, &effective, &positive, &negative);
        } else {
            int32_t resistance = unit->m_unit->m_unitFieldResistances[school];
            total = resistance <= 0 ? 0 : resistance;
            base = total;
        }
    }

    lua_pushnumber(L, base);
    lua_pushnumber(L, total);
    lua_pushnumber(L, positive);
    lua_pushnumber(L, negative);
    return 4;
}

// VERIFY: 7fbda2e6 manual
// OFFSET: 0x00610DE0
static int32_t Script_UnitDefense(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitDefense(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));
    int32_t rank = 0;
    int32_t bonus = 0;

    if (unit && (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || unit->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer())) {
        unit->GetDefenseSkillRank(rank, bonus);
    }

    if (rank + bonus < 0) {
        bonus = -rank;
    }

    lua_pushnumber(L, rank);
    lua_pushnumber(L, bonus);
    return 2;
}

// VERIFY: bc7e1a11 manual
// OFFSET: 0x00610450
static int32_t Script_UnitAttackBothHands(lua_State* L) {
    int32_t bonus[2];
    int32_t rank[2];

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitAttackBothHands(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() || unit->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer())) {
        for (int32_t i = 0; i < 2; i++) {
            unit->GetAttackSkillRank(i, rank[i], bonus[i]);

            if (rank[i] + bonus[i] < 0) {
                bonus[i] = -rank[i];
            }
        }
    } else {
        bonus[0] = 0;
        rank[0] = 0;
        bonus[1] = 0;
        rank[1] = 0;
    }

    for (int32_t i = 0; i < 2; i++) {
        lua_pushnumber(L, rank[i]);
        lua_pushnumber(L, bonus[i]);
    }

    return 4;
}

// VERIFY: 1de5d643 manual
// OFFSET: 0x00611DF0
static int32_t Script_UnitCastingInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCastingInfo(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (!unit) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();
    SpellRec spell;

    if (!g_spellDB.GetRecord(unit->m_castingSpellId, &spell) || static_cast<int32_t>(now - unit->m_castEndTime) >= 0) {
        return 0;
    }

    const char* name = spell.m_name;
    const char* rank = spell.m_nameSubtext;
    const char* displayName = spell.m_attributesExC & 4 ? "" : spell.m_name;
    CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(unit->m_castingObject, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (gameObject && (gameObject->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        const char* text = "";
        LockRec* lock = gameObject->GetLockRec();

        if (lock) {
            for (uint32_t i = 0; i < 3; i++) {
                if (spell.m_effect[i] != 33) {
                    continue;
                }

                for (uint32_t j = 0; j < 8; j++) {
                    if (lock->m_type[j] == 2 && spell.m_effectMiscValue[i] == lock->m_index[j]) {
                        int32_t action = lock->m_action[j];

                        if (action >= 0) {
                            if (action <= 1) {
                                text = gameObject->GetOpenText();
                            } else if (action == 2) {
                                text = gameObject->GetCloseText();
                            }
                        }

                        break;
                    }
                }
            }

            if (text && *text) {
                displayName = text;
            }
        }
    }

    const char* icon = "";
    SpellIconRec* iconRec = g_spellIconDB.GetRecord(spell.m_spellIconID);

    if (iconRec) {
        icon = iconRec->m_textureFilename;
    }

    bool isTradeSkill = (spell.m_attributes & 0x20) != 0;
    bool notInterruptible = unit->IsCastNotInterruptible();
    lua_pushstring(L, name);
    lua_pushstring(L, rank);
    lua_pushstring(L, displayName);
    lua_pushstring(L, icon);
    lua_pushnumber(L, unit->m_castStartTime);
    lua_pushnumber(L, unit->m_castEndTime);
    lua_pushboolean(L, isTradeSkill);
    lua_pushnumber(L, unit->m_castCount);
    lua_pushboolean(L, notInterruptible);
    return 9;
}

// VERIFY: 4a66dce9 manual
// OFFSET: 0x00613140
static int32_t Script_GetRuneCount(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRuneCount(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 7) {
        return 0;
    }

    lua_pushnumber(L, (CGRuneInfo::m_predictedRunes & (1 << slot)) != 0);
    return 1;
}

// VERIFY: 464ea29a manual
// OFFSET: 0x0060CAA0
static int32_t Script_UnitInBattleground(lua_State* L) {
    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    int32_t index = CGRaidInfo::GetMemberIndexByGuid(guid);

    if (index >= 0 && CGRaidInfo::IsPVPRaid()) {
        lua_pushnumber(L, index);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b3269b1e manual
// OFFSET: 0x0060D530
static int32_t Script_UnitCanCooperate(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitCanCooperate(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    const char* otherToken = lua_tolstring(L, 2, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    WOWGUID otherGuid = WOWGUID();
    Script_GetGUIDFromToken(otherToken, otherGuid, false);
    CGUnit_C* other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(otherGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if ((unit && other && unit->CanCooperate(other)) || (!SStrCmpI(token, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(otherGuid)) || (!SStrCmpI(otherToken, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(guid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 256059b3 manual
// OFFSET: 0x0060F9E0
static int32_t Script_UnitLevel(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitLevel(\"unit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    int32_t level;

    if (unit) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        level = unit->m_unit->m_unitFieldLevel;

        if ((player && unit->UnitReaction(player) <= 1 && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= static_cast<int32_t>(unit->m_unit->m_unitFieldLevel) - 10) || unit->IsBossMob()) {
            level = -1;
        } else if (level > 0 && player && (player->m_player->m_playerBytes3[1] || player->m_player->m_playerFakeInebriation) && player->CanAttack(unit)) {
            level -= RoundToInt64(player->GetDrunknessTurnValue() * 5.0f);

            if (level < 1) {
                level = 1;
            }
        }
    } else if (!SStrCmpI(token, "player", STORM_MAX_STR)) {
        level = ClientServices::GetCharacterLevel();
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(guid);

        if (!state) {
            lua_pushnumber(L, 0.0);
            return 1;
        }

        level = state->m_level;
    }

    lua_pushnumber(L, level);
    return 1;
}

// VERIFY: 220e2bdf auto
// OFFSET: 0x00613A60
static int32_t Script_UnitThreatSituation(lua_State* L) {
    uint8_t status = 0;
    WOWGUID guid;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false)) {
        luaL_error(L, "Usage: UnitThreatSituation(\"unit\" [, \"mob\"])");
        return 0;
    }

    if (!guid) {
        return 0;
    }

    if (lua_isstring(L, 2)) {
        CGUnit_C* mob;

        if (!GetUnitByKeyword(lua_tolstring(L, 2, nullptr), &mob, false)) {
            luaL_error(L, "Usage: UnitThreatSituation(\"unit\" [, \"mob\"])");
            return 0;
        }

        if (mob) {
            mob->CalculateThreat(guid, &status, nullptr, nullptr, nullptr);
        }
    } else {
        status = CGUnit_C::GetThreatSituation(guid);
    }

    if (!status) {
        return 0;
    }

    lua_pushnumber(L, status - 1);
    return 1;
}

// VERIFY: 4231ad42 auto
// OFFSET: 0x00614CA0
static int32_t Script_UnitBuff(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitBuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    info.m_filter = (info.m_filter & ~0x3) | 0x1;
    return PushUnitAura(L, &info);
}

// VERIFY: 81f01547 auto
// OFFSET: 0x00614CF0
static int32_t Script_UnitDebuff(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitDebuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    info.m_filter = (info.m_filter & ~0x1) | 0x2;
    return PushUnitAura(L, &info);
}

// VERIFY: 2c76197b auto
// OFFSET: 0x00614D40
static int32_t Script_UnitAura(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: UnitAura(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    return PushUnitAura(L, &info);
}

// VERIFY: 2717e6ac auto
// OFFSET: 0x00613740
static int32_t Script_UnitHasVehicleUI(lua_State* L) {
    VehicleSeatRec* seat = GetVehicleSeatRecAsPassengerFromLua(L);

    if (seat) {
        lua_pushboolean(L, seat->m_flags & 0x20000000);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// VERIFY: 18bea332 auto
// OFFSET: 0x0060C3D0
static int32_t Script_UnitIsUnit(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsUnit(\"unit\", \"otherUnit\")");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    const char* otherToken = lua_tolstring(L, 2, nullptr);

    if (SStrCmpI(token, otherToken, STORM_MAX_STR)) {
        WOWGUID guid = GetGuidByKeyword(token);
        WOWGUID otherGuid = GetGuidByKeyword(otherToken);

        if (!guid || guid.m_guid == 0xFFFFFFFFFFFFFFFEull || guid != otherGuid) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// VERIFY: ad8c618d manual
// OFFSET: 0x00610FB0
static int32_t Script_UnitCharacterPoints(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitCharacterPoints(\"unit\")");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        for (int32_t i = 2; i; i--) {
            lua_pushnumber(L, 0.0);
        }

        return 2;
    }

    for (uint32_t i = 0; i < 2; i++) {
        int32_t points = unit->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? static_cast<CGPlayer_C*>(unit)->m_player->m_playerCharacterPoints[i] : 0;
        lua_pushnumber(L, points);
    }

    return 2;
}

// VERIFY: 2b8a7058 manual
// OFFSET: 0x00612090
static int32_t Script_UnitChannelInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitChannelInfo(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (!unit) {
        return 0;
    }

    uint32_t now = OsGetAsyncTimeMs();
    SpellRec spell;

    if (!g_spellDB.GetRecord(unit->m_channelSpellId, &spell) || static_cast<int32_t>(now - unit->m_channelEndTime) >= 0) {
        return 0;
    }

    const char* name = spell.m_name;
    const char* rank = spell.m_nameSubtext;
    const char* displayName;

    if (SStrCmpI(lua_tolstring(L, 1, nullptr), "player", STORM_MAX_STR) || (spell.m_attributesEx & 0x20000000)) {
        displayName = name;
    } else {
        displayName = FrameScript_GetText("CHANNELING", -1, GENDER_NOT_APPLICABLE);
    }

    const char* icon = "";
    SpellIconRec* iconRec = g_spellIconDB.GetRecord(spell.m_spellIconID);

    if (iconRec) {
        icon = iconRec->m_textureFilename;
    }

    bool tradeSkill = (spell.m_attributes & 0x20) != 0;
    bool notInterruptible = unit->IsCastNotInterruptible();
    lua_pushstring(L, name);
    lua_pushstring(L, rank);
    lua_pushstring(L, displayName);
    lua_pushstring(L, icon);
    lua_pushnumber(L, unit->m_channelStartTime);
    lua_pushnumber(L, unit->m_channelEndTime);
    lua_pushboolean(L, tradeSkill);
    lua_pushboolean(L, notInterruptible);
    return 8;
}

// VERIFY: 55ecc993 manual
// OFFSET: 0x00611670
static int32_t Script_GetComboPoints(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetComboPoints(\"unit\"[, \"target\"]");
    }

    WOWGUID unit = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), unit, false);
    WOWGUID target = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 2, nullptr), target, false);
    double points = 0.0;

    if (unit == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        uint8_t classId = player ? player->m_unit->m_unitFieldBytes0[1] : 0;

        if (!player || classId == 4 || classId == 11) {
            points = CGGameUI::GetComboPoints(target);
        }
    } else if (unit == CGPetInfo::GetPet(0)) {
        points = CGPetInfo::GetComboPoints(target);
    }

    lua_pushnumber(L, points);
    return 1;
}

// VERIFY: 00eaa6db manual
// OFFSET: 0x00611CB0
static int32_t Script_GetPVPRankInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPVPRankInfo(rank [, unit])");
    }

    int32_t rank = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t faction = 0;

    if (lua_isnumber(L, 2)) {
        faction = static_cast<int32_t>(lua_tonumber(L, 2));
    } else {
        CGPlayer_C* player = nullptr;
        bool valid = true;

        if (lua_isstring(L, 2)) {
            player = static_cast<CGPlayer_C*>(GetUnitFromName(lua_tolstring(L, 2, nullptr)));
            valid = player && (player->m_obj->m_type & TYPEMASK_PLAYER);
        } else {
            player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        }

        if (valid && player) {
            faction = player->GetPVPFactionIndex();
        }
    }

    char token[32];
    const char* text;

    if (static_cast<uint32_t>(rank - 1) <= 17 && (SStrPrintf(token, sizeof(token), "PVP_RANK_%d_%d", rank, faction), (text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE)) != nullptr) && *text) {
        lua_pushstring(L, text);
        lua_pushnumber(L, rank >= 5 ? rank - 4 : rank - 5);
    } else {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
    }

    return 2;
}

// VERIFY: b2631c51 auto
// OFFSET: 0x00608560
static int32_t Script_GetPVPRankProgress(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// VERIFY: 4efe5d27 manual
// OFFSET: 0x0060C810
static int32_t Script_UnitGroupRolesAssigned(lua_State* L) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(lua_tolstring(L, 1, nullptr), guid, false);
    CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);
    uint32_t role = member ? member->m_groupRole : CGPartyInfo::GetGroupRole(guid);
    lua_pushboolean(L, (role >> 1) & 1);
    lua_pushboolean(L, (role >> 2) & 1);
    lua_pushboolean(L, (role >> 3) & 1);
    return 3;
}

// VERIFY: 50d0f834 manual
// OFFSET: 0x006131E0
static int32_t Script_GetRuneType(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_GetRuneType(slot)");
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(index) > 7 || !CGRuneInfo::IsRuneIndexValid(index)) {
        return 0;
    }

    lua_pushnumber(L, CGRuneInfo::GetRuneTypeByIndex(index, false) + 1);
    return 1;
}

// VERIFY: a37de20f auto
// OFFSET: 0x0060E1B0
static int32_t Script_GetSpellCritChanceFromIntellect(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetSpellCritChanceFromIntellect(\"unit\")");
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, nullptr));

    if (unit && (unit->IsActivePlayer() || unit->m_unit->m_unitFieldPetnumber)) {
        lua_pushnumber(L, unit->GetSpellChanceToCrit());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 7b6edbe8 manual
// OFFSET: 0x0060E310
static int32_t Script_GetSpellBonusDamage(lua_State* L) {
    uint32_t school = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (school > 6) {
        luaL_error(L, "Usage: GetSpellBonusDamage(school)");
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        int32_t positive = player->GetModDamageDonePos(school);
        lua_pushnumber(L, player->GetModDamageDoneNeg(school) + positive);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// VERIFY: 5e857cf7 manual
// OFFSET: 0x00613020
static int32_t Script_GetRuneCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRuneCooldown(slot)");
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 7) {
        luaL_error(L, "Invalid slot");
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t lastUsed = CGRuneInfo::GetRuneLastUsedTime(slot);
    float regen = player->m_player->m_playerRuneRegen1[CGRuneInfo::GetRuneTypeByIndex(slot, false)];
    float duration = regen > 0.01f ? 1.0f / regen : 0.0f;
    lua_pushnumber(L, lastUsed * 0.001);
    lua_pushnumber(L, duration);
    lua_pushboolean(L, lastUsed == 0);
    return 3;
}

// STATIC: 0x00AD2184
// VERIFY: cabd9576 manual
// OFFSET: 0x0060CE20
static int32_t Script_UnitIsPVP(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UnitIsPVP(\"unit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID stateGuid = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    bool pvp;

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        pvp = static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldBytes2[1] & 1;
    } else if (object && (object->m_obj->m_type & TYPEMASK_CORPSE)) {
        pvp = static_cast<CGCorpse_C*>(object)->m_corpse->m_corpseFieldFlags & 4;
    } else {
        CGPartyMemberState* state = CGPartyMemberStateRepository::GetState(stateGuid);
        pvp = state && (state->m_flags & 2);
    }

    if (pvp) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: cffb18d2 manual
// OFFSET: 0x0060D3D0
static int32_t Script_UnitIsFriend(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: UnitIsFriend(\"unit\", \"otherUnit\")");
    }

    const char* token = lua_tolstring(L, 1, nullptr);
    const char* otherToken = lua_tolstring(L, 2, nullptr);
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false);
    WOWGUID unitGuid = guid;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    guid = WOWGUID();
    Script_GetGUIDFromToken(otherToken, guid, false);
    WOWGUID otherGuid = guid;
    CGUnit_C* other = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if ((unit && other && unit->IsFriendly(other)) || (!SStrCmpI(token, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(otherGuid)) || (!SStrCmpI(otherToken, "player", STORM_MAX_STR) && CGGameUI::IsRaidMemberOrPet(unitGuid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static FrameScript_Method s_SystemFunctions[] = {
    { "GetTime", &Script_GetTime },
    { "GetGameTime", &Script_GetGameTime },
    { "ConsoleExec", &Script_ConsoleExec },
    { "ReadFile", &Script_ReadFile },
    { "DeleteFile", &Script_DeleteFile },
    { "AppendToFile", &Script_AppendToFile },
    { "GetAccountExpansionLevel", &Script_GetAccountExpansionLevel }
};

// VERIFY: 5cf87403 auto
// OFFSET: 0x0060A120
void SystemRegisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_SystemFunctions) / sizeof(s_SystemFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_SystemFunctions[i].m_name, s_SystemFunctions[i].m_method);
    }
}

// VERIFY: 570706e7 auto
// OFFSET: 0x0060A150
void SystemUnregisterFunctions() {
    for (uint32_t i = 0; i < sizeof(s_SystemFunctions) / sizeof(s_SystemFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_SystemFunctions[i].m_name);
    }
}

// STATIC: 0x00AD21D8
static FrameScript_Method s_UnitFunctions[] = {
    { "UnitExists", &Script_UnitExists },
    { "UnitIsVisible", &Script_UnitIsVisible },
    { "UnitIsUnit", &Script_UnitIsUnit },
    { "UnitIsPlayer", &Script_UnitIsPlayer },
    { "UnitIsInMyGuild", &Script_UnitIsInMyGuild },
    { "UnitIsCorpse", &Script_UnitIsCorpse },
    { "UnitIsPartyLeader", &Script_UnitIsPartyLeader },
    { "UnitGroupRolesAssigned", &Script_UnitGroupRolesAssigned },
    { "UnitIsRaidOfficer", &Script_UnitIsRaidOfficer },
    { "UnitInParty", &Script_UnitInParty },
    { "UnitPlayerOrPetInParty", &Script_UnitPlayerOrPetInParty },
    { "UnitInRaid", &Script_UnitInRaid },
    { "UnitPlayerOrPetInRaid", &Script_UnitPlayerOrPetInRaid },
    { "UnitPlayerControlled", &Script_UnitPlayerControlled },
    { "UnitIsAFK", &Script_UnitIsAFK },
    { "UnitIsDND", &Script_UnitIsDND },
    { "UnitIsPVP", &Script_UnitIsPVP },
    { "UnitIsPVPSanctuary", &Script_UnitIsPVPSanctuary },
    { "UnitIsPVPFreeForAll", &Script_UnitIsPVPFreeForAll },
    { "UnitFactionGroup", &Script_UnitFactionGroup },
    { "UnitReaction", &Script_UnitReaction },
    { "UnitIsEnemy", &Script_UnitIsEnemy },
    { "UnitIsFriend", &Script_UnitIsFriend },
    { "UnitCanCooperate", &Script_UnitCanCooperate },
    { "UnitCanAssist", &Script_UnitCanAssist },
    { "UnitCanAttack", &Script_UnitCanAttack },
    { "UnitIsCharmed", &Script_UnitIsCharmed },
    { "UnitIsPossessed", &Script_UnitIsPossessed },
    { "PlayerCanTeleport", &Script_PlayerCanTeleport },
    { "UnitClassification", &Script_UnitClassification },
    { "UnitSelectionColor", &Script_UnitSelectionColor },
    { "UnitGUID", &Script_UnitGUID },
    { "UnitName", &Script_UnitName },
    { "UnitPVPName", &Script_UnitPVPName },
    { "UnitXP", &Script_UnitXP },
    { "UnitXPMax", &Script_UnitXPMax },
    { "UnitHealth", &Script_UnitHealth },
    { "UnitHealthMax", &Script_UnitHealthMax },
    { "UnitMana", &Script_UnitMana },
    { "UnitManaMax", &Script_UnitManaMax },
    { "UnitPower", &Script_UnitPower },
    { "UnitPowerMax", &Script_UnitPowerMax },
    { "UnitPowerType", &Script_UnitPowerType },
    { "UnitOnTaxi", &Script_UnitOnTaxi },
    { "UnitIsFeignDeath", &Script_UnitIsFeignDeath },
    { "UnitIsDead", &Script_UnitIsDead },
    { "UnitIsGhost", &Script_UnitIsGhost },
    { "UnitIsDeadOrGhost", &Script_UnitIsDeadOrGhost },
    { "UnitIsConnected", &Script_UnitIsConnected },
    { "UnitAffectingCombat", &Script_UnitAffectingCombat },
    { "UnitSex", &Script_UnitSex },
    { "UnitLevel", &Script_UnitLevel },
    { "GetMoney", &Script_GetMoney },
    { "GetHonorCurrency", &Script_GetHonorCurrency },
    { "GetArenaCurrency", &Script_GetArenaCurrency },
    { "UnitRace", &Script_UnitRace },
    { "UnitClass", &Script_UnitClass },
    { "UnitClassBase", &Script_UnitClassBase },
    { "UnitResistance", &Script_UnitResistance },
    { "UnitStat", &Script_UnitStat },
    { "UnitAttackBothHands", &Script_UnitAttackBothHands },
    { "UnitDamage", &Script_UnitDamage },
    { "UnitRangedDamage", &Script_UnitRangedDamage },
    { "UnitRangedAttack", &Script_UnitRangedAttack },
    { "UnitAttackSpeed", &Script_UnitAttackSpeed },
    { "UnitAttackPower", &Script_UnitAttackPower },
    { "UnitRangedAttackPower", &Script_UnitRangedAttackPower },
    { "UnitDefense", &Script_UnitDefense },
    { "UnitArmor", &Script_UnitArmor },
    { "UnitCharacterPoints", &Script_UnitCharacterPoints },
    { "UnitBuff", &Script_UnitBuff },
    { "UnitDebuff", &Script_UnitDebuff },
    { "UnitAura", &Script_UnitAura },
    { "UnitIsTapped", &Script_UnitIsTapped },
    { "UnitIsTappedByPlayer", &Script_UnitIsTappedByPlayer },
    { "UnitIsTappedByAllThreatList", &Script_UnitIsTappedByAllThreatList },
    { "UnitIsTrivial", &Script_UnitIsTrivial },
    { "UnitHasRelicSlot", &Script_UnitHasRelicSlot },
    { "SetPortraitTexture", &Script_SetPortraitTexture },
    { "HasFullControl", &Script_HasFullControl },
    { "GetComboPoints", &Script_GetComboPoints },
    { "IsInGuild", &Script_IsInGuild },
    { "IsGuildLeader", &Script_IsGuildLeader },
    { "IsArenaTeamCaptain", &Script_IsArenaTeamCaptain },
    { "IsInArenaTeam", &Script_IsInArenaTeam },
    { "IsResting", &Script_IsResting },
    { "GetCombatRating", &Script_GetCombatRating },
    { "GetCombatRatingBonus", &Script_GetCombatRatingBonus },
    { "GetMaxCombatRatingBonus", &Script_GetMaxCombatRatingBonus },
    { "GetDodgeChance", &Script_GetDodgeChance },
    { "GetBlockChance", &Script_GetBlockChance },
    { "GetShieldBlock", &Script_GetShieldBlock },
    { "GetParryChance", &Script_GetParryChance },
    { "GetCritChanceFromAgility", &Script_GetCritChanceFromAgility },
    { "GetSpellCritChanceFromIntellect", &Script_GetSpellCritChanceFromIntellect },
    { "GetCritChance", &Script_GetCritChance },
    { "GetRangedCritChance", &Script_GetRangedCritChance },
    { "GetSpellCritChance", &Script_GetSpellCritChance },
    { "GetSpellBonusDamage", &Script_GetSpellBonusDamage },
    { "GetSpellBonusHealing", &Script_GetSpellBonusHealing },
    { "GetPetSpellBonusDamage", &Script_GetPetSpellBonusDamage },
    { "GetSpellPenetration", &Script_GetSpellPenetration },
    { "GetArmorPenetration", &Script_GetArmorPenetration },
    { "GetAttackPowerForStat", &Script_GetAttackPowerForStat },
    { "UnitCreatureType", &Script_UnitCreatureType },
    { "UnitCreatureFamily", &Script_UnitCreatureFamily },
    { "GetResSicknessDuration", &Script_GetResSicknessDuration },
    { "GetPVPSessionStats", &Script_GetPVPSessionStats },
    { "GetPVPYesterdayStats", &Script_GetPVPYesterdayStats },
    { "GetPVPLifetimeStats", &Script_GetPVPLifetimeStats },
    { "UnitPVPRank", &Script_UnitPVPRank },
    { "GetPVPRankInfo", &Script_GetPVPRankInfo },
    { "GetPVPRankProgress", &Script_GetPVPRankProgress },
    { "UnitCastingInfo", &Script_UnitCastingInfo },
    { "UnitChannelInfo", &Script_UnitChannelInfo },
    { "IsLoggedIn", &Script_IsLoggedIn },
    { "IsFlyableArea", &Script_IsFlyableArea },
    { "IsIndoors", &Script_IsIndoors },
    { "IsOutdoors", &Script_IsOutdoors },
    { "IsOutOfBounds", &Script_IsOutOfBounds },
    { "IsFalling", &Script_IsFalling },
    { "IsSwimming", &Script_IsSwimming },
    { "IsFlying", &Script_IsFlying },
    { "IsMounted", &Script_IsMounted },
    { "IsStealthed", &Script_IsStealthed },
    { "UnitIsSameServer", &Script_UnitIsSameServer },
    { "GetUnitHealthModifier", &Script_GetUnitHealthModifier },
    { "GetUnitMaxHealthModifier", &Script_GetUnitMaxHealthModifier },
    { "GetUnitPowerModifier", &Script_GetUnitPowerModifier },
    { "GetUnitHealthRegenRateFromSpirit", &Script_GetUnitHealthRegenRateFromSpirit },
    { "GetUnitManaRegenRateFromSpirit", &Script_GetUnitManaRegenRateFromSpirit },
    { "GetManaRegen", &Script_GetManaRegen },
    { "GetPowerRegen", &Script_GetPowerRegen },
    { "GetRuneCooldown", &Script_GetRuneCooldown },
    { "GetRuneCount", &Script_GetRuneCount },
    { "GetRuneType", &Script_GetRuneType },
    { "ReportPlayerIsPVPAFK", &Script_ReportPlayerIsPVPAFK },
    { "PlayerIsPVPInactive", &Script_PlayerIsPVPInactive },
    { "GetExpertise", &Script_GetExpertise },
    { "GetExpertisePercent", &Script_GetExpertisePercent },
    { "UnitInBattleground", &Script_UnitInBattleground },
    { "UnitInRange", &Script_UnitInRange },
    { "GetUnitSpeed", &Script_GetUnitSpeed },
    { "GetUnitPitch", &Script_GetUnitPitch },
    { "UnitInVehicle", &Script_UnitInVehicle },
    { "UnitUsingVehicle", &Script_UnitUsingVehicle },
    { "UnitControllingVehicle", &Script_UnitControllingVehicle },
    { "UnitInVehicleControlSeat", &Script_UnitInVehicleControlSeat },
    { "UnitHasVehicleUI", &Script_UnitHasVehicleUI },
    { "UnitTargetsVehicleInRaidUI", &Script_UnitTargetsVehicleInRaidUI },
    { "UnitVehicleSkin", &Script_UnitVehicleSkin },
    { "UnitVehicleSeatCount", &Script_UnitVehicleSeatCount },
    { "UnitVehicleSeatInfo", &Script_UnitVehicleSeatInfo },
    { "UnitSwitchToVehicleSeat", &Script_UnitSwitchToVehicleSeat },
    { "CanSwitchVehicleSeat", &Script_CanSwitchVehicleSeat },
    { "GetVehicleUIIndicator", &Script_GetVehicleUIIndicator },
    { "GetVehicleUIIndicatorSeat", &Script_GetVehicleUIIndicatorSeat },
    { "UnitThreatSituation", &Script_UnitThreatSituation },
    { "UnitDetailedThreatSituation", &Script_UnitDetailedThreatSituation },
    { "UnitIsControlling", &Script_UnitIsControlling },
    { "EjectPassengerFromSeat", &Script_EjectPassengerFromSeat },
    { "CanEjectPassengerFromSeat", &Script_CanEjectPassengerFromSeat },
    { "RespondInstanceLock", &Script_RespondInstanceLock },
    { "GetPlayerFacing", &Script_GetPlayerFacing },
    { "GetPlayerInfoByGUID", &Script_GetPlayerInfoByGUID },
    { "GetItemStats", &Script_GetItemStats },
    { "GetItemStatDelta", &Script_GetItemStatDelta },
    { "IsXPUserDisabled", &Script_IsXPUserDisabled },
    { "FillLocalizedClassList", &Script_FillLocalizedClassList },
};

// VERIFY: 24f40cd0 auto
// OFFSET: 0x0060A170
void ScriptEventsRegisterFunctions() {
    SystemRegisterFunctions();

    for (uint32_t i = 0; i < sizeof(s_UnitFunctions) / sizeof(s_UnitFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_UnitFunctions[i].m_name, s_UnitFunctions[i].m_method);
    }
}
