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

int32_t Script_GetAccountExpansionLevel(lua_State* L);
int32_t Script_UnitIsUnit(lua_State* L);
int32_t Script_UnitIsInMyGuild(lua_State* L);
int32_t Script_UnitIsPartyLeader(lua_State* L);
int32_t Script_UnitGroupRolesAssigned(lua_State* L);
int32_t Script_UnitIsRaidOfficer(lua_State* L);
int32_t Script_UnitInParty(lua_State* L);
int32_t Script_UnitPlayerOrPetInParty(lua_State* L);
int32_t Script_UnitInRaid(lua_State* L);
int32_t Script_UnitPlayerOrPetInRaid(lua_State* L);
int32_t Script_UnitIsAFK(lua_State* L);
int32_t Script_UnitIsDND(lua_State* L);
int32_t Script_UnitIsPVP(lua_State* L);
int32_t Script_UnitIsPVPSanctuary(lua_State* L);
int32_t Script_UnitIsPVPFreeForAll(lua_State* L);
int32_t Script_UnitFactionGroup(lua_State* L);
int32_t Script_UnitReaction(lua_State* L);
int32_t Script_UnitIsEnemy(lua_State* L);
int32_t Script_UnitIsFriend(lua_State* L);
int32_t Script_UnitCanCooperate(lua_State* L);
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
int32_t Script_UnitLevel(lua_State* L);
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
int32_t Script_UnitCharacterPoints(lua_State* L);
int32_t Script_UnitBuff(lua_State* L);
int32_t Script_UnitDebuff(lua_State* L);
int32_t Script_UnitAura(lua_State* L);
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
int32_t Script_GetSpellCritChanceFromIntellect(lua_State* L);
int32_t Script_GetCritChance(lua_State* L);
int32_t Script_GetRangedCritChance(lua_State* L);
int32_t Script_GetSpellCritChance(lua_State* L);
int32_t Script_GetSpellBonusDamage(lua_State* L);
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
int32_t Script_GetPVPRankProgress(lua_State* L);
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
int32_t Script_GetRuneCooldown(lua_State* L);
int32_t Script_GetRuneCount(lua_State* L);
int32_t Script_GetRuneType(lua_State* L);
int32_t Script_ReportPlayerIsPVPAFK(lua_State* L);
int32_t Script_PlayerIsPVPInactive(lua_State* L);
int32_t Script_GetExpertise(lua_State* L);
int32_t Script_GetExpertisePercent(lua_State* L);
int32_t Script_UnitInBattleground(lua_State* L);
int32_t Script_UnitInRange(lua_State* L);
int32_t Script_GetUnitSpeed(lua_State* L);
int32_t Script_GetUnitPitch(lua_State* L);
int32_t Script_UnitInVehicle(lua_State* L);
int32_t Script_UnitUsingVehicle(lua_State* L);
int32_t Script_UnitControllingVehicle(lua_State* L);
int32_t Script_UnitInVehicleControlSeat(lua_State* L);
int32_t Script_UnitHasVehicleUI(lua_State* L);
int32_t Script_UnitTargetsVehicleInRaidUI(lua_State* L);
int32_t Script_UnitVehicleSkin(lua_State* L);
int32_t Script_UnitVehicleSeatCount(lua_State* L);
int32_t Script_UnitVehicleSeatInfo(lua_State* L);
int32_t Script_UnitSwitchToVehicleSeat(lua_State* L);
int32_t Script_CanSwitchVehicleSeat(lua_State* L);
int32_t Script_GetVehicleUIIndicator(lua_State* L);
int32_t Script_GetVehicleUIIndicatorSeat(lua_State* L);
int32_t Script_UnitThreatSituation(lua_State* L);
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

// STATIC: 0x009FE7EC
static const int32_t s_unitSexValues[3] = { 2, 3, 1 };

// STATIC: 0x00AD21BC
static const char* s_unitClassificationTokens[7] = { "normal", "elite", "rareelite", "worldboss", "rare", "trivial", nullptr };

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

// STATIC: 0x00AD2184
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

// VERIFY: f8cb4c3d manual
// OFFSET: 0x0060E740
int32_t Script_UnitName(lua_State* L) {
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

// VERIFY: 30a48f1b manual
// OFFSET: 0x0060ED40
int32_t Script_UnitPower(lua_State* L) {
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

// VERIFY: 345565e8 manual
// OFFSET: 0x0060EF40
int32_t Script_UnitPowerMax(lua_State* L) {
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

// VERIFY: 8b337fee manual
// OFFSET: 0x0060F100
int32_t Script_UnitPowerType(lua_State* L) {
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

// VERIFY: 87cc1310 manual
// OFFSET: 0x0060D0A0
int32_t Script_UnitFactionGroup(lua_State* L) {
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
