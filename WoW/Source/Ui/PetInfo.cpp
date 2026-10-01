#include "PetInfo.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "Engine/Source/Os/TimeManager.h"

// STATIC: 0x00C234DC
int32_t CGPetInfo::m_attackingTarget;

// STATIC: 0x00C234E0
int32_t CGPetInfo::m_possessSpell;

// STATIC: 0x00C234E4
uint8_t CGPetInfo::m_comboPoints;

// STATIC: 0x00C234E8
WOWGUID CGPetInfo::m_comboTarget;

// STATIC: 0x00C234F8
TSGrowableArray<WOWGUID> s_pets;

// STATIC: 0x00C23508
uint32_t CGPetInfo::m_actions[10];

// STATIC: 0x00C234C8
uint32_t CGPetInfo::m_petMode;

// STATIC: 0x00A1A860
static const char s_petModeTokens[3][32] = { "PASSIVE", "DEFENSIVE", "AGGRESSIVE" };

// STATIC: 0x00A1A8C0
static const char s_petOrderTokens[4][32] = { "WAIT", "FOLLOW", "ATTACK", "DISMISS" };

// STATIC: 0x00C234CC
uint32_t CGPetInfo::m_expirationTime;

// STATIC: 0x00C234D0
WOWGUID CGPetInfo::m_pet;

// STATIC: 0x00C234D8
uint32_t CGPetInfo::m_petBarState;

// STATIC: 0x00C23530
static TSGrowableArray<uint32_t> s_petSpellActions;

// VERIFY: 0f361a6f auto
// OFFSET: 0x005D3080
void CGPetInfo::HideGrid() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_HIDEGRID, nullptr);
}

// VERIFY: b4168bca manual
// OFFSET: 0x005D3390
WOWGUID CGPetInfo::GetPet(uint32_t index) {
    if (index >= s_pets.m_count) {
        return WOWGUID();
    }

    return s_pets.m_data[index];
}

// VERIFY: b990bce8 manual
// OFFSET: 0x005D4650
void CGPetInfo::PetClearTarget() {
    if (!CGPetInfo::m_attackingTarget) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_STOP_ATTACK));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPetInfo::m_attackingTarget = 0;
}

// VERIFY: 9cd6d950 auto
// OFFSET: 0x005D3520
uint8_t CGPetInfo::GetComboPoints(WOWGUID target) {
    if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    if (target == CGPetInfo::m_comboTarget) {
        return CGPetInfo::m_comboPoints;
    }

    return 0;
}

// VERIFY: 61b1e561 manual
// OFFSET: 0x005D6E60
void SendRequestPetInfo() {
    CGPetInfo::SetPet(WOWGUID(), 0, 0);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

    if (!pet) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PET_INFO));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGPetInfo::FindPossessedSpell();
}

// VERIFY: cf365005 auto
// OFFSET: 0x005D3050
const char* CGPetInfo::GetModeToken(uint32_t mode) {
    return s_petModeTokens[mode];
}

// VERIFY: 5d255e3f auto
// OFFSET: 0x005D3060
const char* CGPetInfo::GetOrdersToken(uint32_t order) {
    return s_petOrderTokens[order];
}

// VERIFY: 804722ba auto
// OFFSET: 0x005D3090
void CGPetInfo::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE_COOLDOWN, nullptr);
}

// VERIFY: 5862d15e manual
// OFFSET: 0x005D33C0
WOWGUID CGPetInfo::GetTarget() {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet) {
        return pet->m_unit->m_unitFieldTarget;
    }

    return WOWGUID();
}

// VERIFY: 41802278 manual
// OFFSET: 0x005D64F0
void CGPetInfo::SetPetCount(uint32_t count) {
    s_pets.SetCount(count);
}

// VERIFY: 5e3e4434 auto
// OFFSET: 0x005D41B0
int32_t CGPetInfo::IsAttackAction(uint32_t index) {
    uint32_t* action = CGPetInfo::GetPet(0) && index < sizeof(CGPetInfo::m_actions) / sizeof(CGPetInfo::m_actions[0]) ? &CGPetInfo::m_actions[index] : nullptr;

    if (action && *action && (*action & 0x3F000000) == 0x07000000 && (*action & 0xFFFFFF) == 2) {
        return 1;
    }

    return 0;
}

// VERIFY: c296d650 auto
// OFFSET: 0x005D6D90
void CGPetInfo::InitializeGame() {
    CGPetInfo::m_pet = WOWGUID();
    CGPetInfo::m_expirationTime = 0;
    CGPetInfo::m_petBarState = 0;
    CGPetInfo::m_comboTarget = WOWGUID();
    CGPetInfo::m_comboPoints = 0;
    CGPetInfo::SetPetCount(0);
    ClientServices::SetMessageHandler(SMSG_PET_SPELLS_MESSAGE, Packet_SMSG_PET_SPELLS, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_LEARNED_SPELLS, Packet_Group_7, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_UNLEARNED_SPELLS, Packet_Group_7, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_MODE, Packet_SMSG_PET_MODE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_ACTION_FEEDBACK, Packet_SMSG_PET_ACTION_FEEDBACK, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_BROKEN, Packet_SMSG_PET_BROKEN, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_RENAMEABLE, Packet_SMSG_PET_RENAMEABLE, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_UPDATE_COMBO_POINTS, Packet_SMSG_PET_UPDATE_COMBO_POINTS, nullptr);
    ClientServices::SetMessageHandler(SMSG_PET_GUIDS, Packet_SMSG_PET_GUIDS, nullptr);
}

// VERIFY: b893299b manual
// OFFSET: 0x005D35B0
int32_t CGPetInfo::IsPetBarUsed() {
    if (!s_pets.Count() || !s_pets.m_data[0]) {
        return 0;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pets.m_data[0], TYPEMASK_UNIT, __FILE__, __LINE__));
    return !pet || (pet->IsPetBarUsed() && pet->m_unit->m_unitFieldHealth > 0 && (!CGPetInfo::m_petBarState || CGPetInfo::m_possessSpell || pet->CanGainXP()));
}

// VERIFY: 2c073ab7 manual
// OFFSET: 0x005D3560
uint32_t* CGPetInfo::FindSpellByID(int32_t spellId) {
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t action = CGPetInfo::m_actions[i];
        uint32_t type = (action >> 24) & 0x3F;

        if ((type == 1 || (type >= 8 && type <= 0x11)) && static_cast<int32_t>(action & 0xFFFFFF) == spellId) {
            return &CGPetInfo::m_actions[i];
        }
    }

    return nullptr;
}

// VERIFY: 16cfdcd2 auto
// OFFSET: 0x005D30E0
void CGPetInfo::SetPossessedSpell(int32_t spellId) {
    int32_t previous = CGPetInfo::m_possessSpell;
    CGPetInfo::m_possessSpell = spellId;

    if (spellId) {
        CGPetInfo::m_petBarState = 1;
    }

    if (previous != spellId) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_BONUS_ACTIONBAR, nullptr);
    }
}

// VERIFY: 0082bae7 manual
// OFFSET: 0x005D3410
uint32_t* CGPetInfo::GetPetSpellAction(int32_t spellId) {
    for (uint32_t i = s_petSpellActions.m_count; i; ) {
        i--;

        if ((s_petSpellActions.m_data[i] & 0xFFFFFF) == static_cast<uint32_t>(spellId)) {
            return &s_petSpellActions.m_data[i];
        }
    }

    return nullptr;
}

// VERIFY: b235dc3e manual
// OFFSET: 0x005D3450
void CGPetInfo::UpdatePetSpellAction(const uint32_t* action) {
    uint32_t value = *action;

    if ((value & 0x3F000000) != 0x1000000) {
        return;
    }

    for (uint32_t i = s_petSpellActions.m_count; i; ) {
        i--;

        if ((s_petSpellActions.m_data[i] & 0x3FFFFFFF) == (value & 0x3FFFFFFF)) {
            s_petSpellActions.m_data[i] = value;
            return;
        }
    }
}

// VERIFY: 9c92f690 manual
// OFFSET: 0x005D5E10
void CGPetInfo::SetSpellAutocast(int32_t spellId, int32_t enable) {
    if (!CGGameUI::CanPerformAction(2) || !CGPetInfo::PetActionsUsable(false)) {
        return;
    }

    uint32_t* action = CGPetInfo::GetPetSpellAction(spellId);

    if (!action || !(*action & 0x80000000)) {
        return;
    }

    if (enable < 0) {
        enable = !(*action & 0x40000000);
    }

    if (enable) {
        *action |= 0x40000000;
    } else {
        *action &= ~0x40000000;
    }

    for (uint32_t i = 0; i < 10; i++) {
        if (!((*action ^ CGPetInfo::m_actions[i]) & 0x3FFFFFFF)) {
            if (*action & 0x40000000) {
                CGPetInfo::m_actions[i] |= 0x40000000;
            } else {
                CGPetInfo::m_actions[i] &= ~0x40000000;
            }
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_SPELL_AUTOCAST));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(static_cast<uint32_t>(spellId));
    msg.Put(static_cast<uint8_t>((*action & 0x40000000) != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);

    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, nullptr);
}

// VERIFY: c40a54ec auto
// OFFSET: 0x005D2FF0
void CGPetInfo::SetPetMode(uint32_t mode) {
    CGPetInfo::m_petMode = mode | (CGPetInfo::m_petMode & 0xFFFFFF00);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, nullptr);
}

// VERIFY: a72652ae auto
// OFFSET: 0x005D3020
void CGPetInfo::SetPetOrders(uint32_t orders) {
    CGPetInfo::m_petMode = (orders << 8) | (CGPetInfo::m_petMode & 0x80000FF);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, nullptr);
}

// VERIFY: d8b1283c manual
// OFFSET: 0x005D3E80
bool CGPetInfo::PetActionsUsable(bool ignoreControl) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->m_unit->m_unitFieldCharmedby.m_guid) {
        return false;
    }

    if (player->GetFarSightGuid() && player->GetFarSightGuid() != CGPetInfo::GetPet(0).m_guid) {
        return false;
    }

    for (uint32_t i = 0; i < s_pets.m_count; i++) {
        CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!pet) {
            continue;
        }

        const WOWGUID& owner = pet->m_unit->m_unitFieldCharmedby.m_guid ? pet->m_unit->m_unitFieldCharmedby : pet->m_unit->m_unitFieldSummonedby;

        if (owner == ClntObjMgrGetActivePlayer() && (ignoreControl || pet->IsControllable())) {
            if (!(CGPetInfo::m_petMode & 0x8000000)) {
                return true;
            }

            if (player->GetTransportGUID() == pet->m_obj->m_guid.m_guid) {
                return true;
            }
        }
    }

    return false;
}

// VERIFY: db3a399f manual
// OFFSET: 0x005D4210
void CGPetInfo::SendPetAction(const uint32_t& action, const WOWGUID& target, int32_t a3) {
    if (!CGGameUI::CanPerformAction(2)) {
        return;
    }

    WOWGUID actionTarget = (a3 || target.m_guid) ? target : CGGameUI::m_lockedTarget;

    switch ((action >> 24) & 0x3F) {
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17: {
            if (!CGPetInfo::PetActionsUsable(true)) {
                return;
            }

            SpellRec spell;

            if (!g_spellDB.GetRecord(action & 0xFFFFFF, &spell)) {
                return;
            }

            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (!player) {
                return;
            }

            CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (Spell_C_IsActiveSpell(&spell, pet)) {
                if (!Spell_C_WasSpellRecentlyUsed(spell.m_id, 1)) {
                    Spell_C_CancelAura(&spell, pet, -1);
                }

                return;
            }

            uint32_t pacified = 0;
            uint32_t silenced = 0;
            uint32_t disarmed = 0;

            if (s_pets.m_count) {
                for (uint32_t i = 0; i < s_pets.m_count; i++) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (!unit) {
                        return;
                    }

                    if (!(spell.m_attributesExE & 0x8) && (unit->m_unit->m_unitFieldFlags & 0x40000)) {
                        pacified++;
                    } else if (!(spell.m_attributesExE & 0x20000) && (unit->m_unit->m_unitFieldFlags & 0x800000)) {
                        silenced++;
                    } else if (!(spell.m_attributesExE & 0x40000) && (unit->m_unit->m_unitFieldFlags & 0x400000)) {
                        disarmed++;
                    }
                }

                if ((pacified && pacified == s_pets.m_count) || (silenced && silenced == s_pets.m_count) || (disarmed && disarmed == s_pets.m_count)) {
                    return;
                }
            }

            if (!(spell.m_attributesExD & 0x20) && player->GetPossessedUnit() != pet) {
                Spell_C_StartSpellCooldown(&spell, pet);
                break;
            }

            Spell_C_CastSpell(pet, spell.m_id, nullptr, actionTarget, nullptr, false);
            return;
        }

        case 6:
            if (!CGPetInfo::PetActionsUsable(false)) {
                return;
            }

            CGPetInfo::SetPetMode(action & 0xFFFFFF);
            break;

        case 7: {
            if (!CGPetInfo::PetActionsUsable(false)) {
                return;
            }

            int32_t orders = action & 0xFFFFFF;

            if (orders <= 1) {
                CGPetInfo::SetPetOrders(orders);
                break;
            }

            if (orders != 2) {
                break;
            }

            for (uint32_t i = 0; i < s_pets.m_count; i++) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(i), TYPEMASK_UNIT, __FILE__, __LINE__));

                if (unit && unit->VerifyAttackIconPressed(&actionTarget, a3, 1, 1, false)) {
                    CGPetInfo::m_attackingTarget = 1;
                }
            }

            if (!CGPetInfo::m_attackingTarget) {
                return;
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, nullptr);
            break;
        }

        default:
            if (!CGPetInfo::PetActionsUsable(false)) {
                return;
            }

            break;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(action);
    msg.Put(actionTarget.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 4d63ce45 manual
// OFFSET: 0x005D62A0
void CGPetInfo::FindPossessedSpell() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    int32_t onCharmVehicle = 0;

    if (player->GetTransportGUID()) {
        const WOWGUID& charm = player->m_unit->m_unitFieldCharm.m_guid ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (player->GetTransportGUID() == charm.m_guid) {
            onCharmVehicle = 1;
        }
    }

    SpellRec spell;

    for (uint32_t i = 0; i < player->GetAuraCount(); i++) {
        AuraData* aura = i < player->GetAuraCount() ? &player->m_auras[i] : nullptr;

        if ((aura->m_flags & 0x80) || !g_spellDB.GetRecord(aura->m_spellId, &spell)) {
            continue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            int32_t auraType = spell.m_effectAura[j];

            if (auraType == 2 || auraType == 128 || auraType == 6 || (auraType == 236 && onCharmVehicle)) {
                CGPetInfo::SetPossessedSpell(aura->m_spellId);
                return;
            }
        }

        for (uint32_t j = 0; j < 3; j++) {
            if (spell.m_effect[j] != 28) {
                continue;
            }

            SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(spell.m_effectMiscValueB[j]);

            if (properties && properties->m_control == 3) {
                CGPetInfo::SetPossessedSpell(aura->m_spellId);
                return;
            }
        }
    }
}

// VERIFY: 81b39234 manual
// OFFSET: 0x005D6B90
int32_t Packet_SMSG_PET_SPELLS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    for (uint32_t i = 0; i < 10; i++) {
        CGPetInfo::m_actions[i] = 0;
    }

    s_petSpellActions.SetCount(0);
    Spell_C_ClearPetCooldowns();

    uint32_t expiration = 0;
    uint16_t family = 0;
    uint64_t pet;
    msg->Get(pet);

    if (pet) {
        msg->Get(family);
        msg->Get(expiration);
        uint32_t mode;
        msg->Get(mode);
        CGPetInfo::m_petMode = mode;

        for (uint32_t i = 0; i < 10; i++) {
            uint32_t action;
            msg->Get(action);
            uint32_t value = action;
            CGPetInfo::SetAction(i, value, 0);

            if (((action >> 24) & 0x3F) >= 8 && ((action >> 24) & 0x3F) <= 17) {
                CGSpellBook::AddSpellToNameCache(action & 0xFFFFFF, 1);
            }
        }

        uint8_t count;
        msg->Get(count);
        s_petSpellActions.SetCount(count);
        ClearPetSpellInfo();

        for (uint32_t i = 0; i < count; i++) {
            uint32_t action;
            msg->Get(action);
            s_petSpellActions.m_data[i] = action;
            CGSpellBook::AddPetSpell(action & 0xFFFFFF);
        }

        msg->Get(count);

        for (uint32_t i = 0; i < count; i++) {
            uint32_t spellId;
            msg->Get(spellId);
            uint16_t category;
            msg->Get(category);
            uint32_t cooldown;
            msg->Get(cooldown);
            uint32_t categoryCooldown;
            msg->Get(categoryCooldown);
            bool onHold = static_cast<int32_t>(categoryCooldown) < 0;
            categoryCooldown &= ~0x80000000;
            Spell_C_SetCooldownLeft(spellId, 0, category, cooldown, categoryCooldown, onHold, 1);
        }
    } else {
        ClearPetSpellInfo();
    }

    CGPetInfo::SetPet(WOWGUID{ pet }, expiration, family);
    CGSpellBook::UpdateSpells(0, 1, 0);
    CGClassTrainer::RefreshList();
    return 1;
}

// VERIFY: 2c74fc11 manual
// OFFSET: 0x005D3240
static int32_t PetTempActionBarCallback(uint32_t index, const void* data) {
    return index < 10 ? index | 0x10000000 : 0;
}

// VERIFY: adc6dd8d auto
// OFFSET: 0x005D34B0
void CGPetInfo::SetComboPoints(WOWGUID target, uint8_t points) {
    CGPetInfo::m_comboTarget = target;
    CGPetInfo::m_comboPoints = points;
    CGSpellBook::UpdateUsable();
    Script_SendUnitSignal(CGPetInfo::GetPet(0), 149);
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE_USABLE, nullptr);
}

// VERIFY: 1003d217 manual
// OFFSET: 0x005D4AD0
void CGPetInfo::UpdatePetTempBar() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t active = 0;

    if (player && player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && player->m_player->m_playerFarsight.m_guid && player->GetFarSightGuid() == CGPetInfo::GetPet(0).m_guid && CGPetInfo::IsPetBarUsed()) {
        active = 1;
        CVehicle_C* vehicle;
        VehicleSeatRec* seat;

        if (!s_pets.m_count || !s_pets.m_data[0].m_guid || player->GetTransportGUID() != s_pets.m_data[0].m_guid || !(vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__))->m_vehicle) || !(seat = vehicle->GetVehicleSeatRec(static_cast<uint8_t>(player->m_movementData.m_transportSeat))) || (active = seat->m_vehicleAbilityDisplay == 1) != 0) {
            CGActionBar::UpdateTemporaryActionBar(PetTempActionBarCallback, nullptr);
        }
    }

    CGActionBar::SetTemporaryActionBarFlags(1, active);
    CGActionBar::UpdateBonusBar();
    FrameScript_SignalEvent(SCRIPT_EVENT_PET_BAR_UPDATE, nullptr);
}

// VERIFY: 138ffa1d manual
// OFFSET: 0x005D6900
void CGPetInfo::SetPet(WOWGUID pet, uint32_t expiration, int32_t family) {
    int32_t changed = CGPetInfo::m_pet != pet;
    CGPetInfo::m_pet = pet;

    if (pet) {
        uint32_t index = 0;

        if (s_pets.m_count) {
            for (index = 0; index < s_pets.m_count; index++) {
                if (s_pets.m_data[index] == pet) {
                    break;
                }
            }

            if (index) {
                WOWGUID first = s_pets.m_data[0];
                s_pets.m_data[0] = pet;

                if (index < s_pets.m_count) {
                    s_pets.m_data[index] = first;
                }
            }
        }

        if (index == s_pets.m_count) {
            CGPetInfo::SetPetCount(1);

            if (s_pets.m_count) {
                s_pets.m_data[0] = pet;
            }
        }
    } else {
        CGPetInfo::SetPetCount(0);
    }

    CGPetInfo::m_expirationTime = expiration ? expiration + OsGetAsyncTimeMs() : 0;

    if (changed) {
        CGPetInfo::m_petBarState = 0;
        CGPetInfo::m_attackingTarget = 0;

        if (s_pets.m_count && s_pets.m_data[0]) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_pets.m_data[0], TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                CGPetInfo::m_attackingTarget = unit->IsInCombat() != 0;

                if (unit->CanGainXP()) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_PET_UI_CLOSE, nullptr);
                }
            }

            CGTalentInfo::UpdatePetTabInfo(family);
        } else {
            CGPetInfo::SetComboPoints(WOWGUID(), 0);
            CGTalentInfo::UpdatePetTabInfo(0);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_PET_2, "%s", "player");
    }

    CGPetInfo::UpdatePetTempBar();

    if (Spell_C_IsModal(CGPetInfo::GetPet(0)) && (changed || !s_pets.m_count || !s_pets.m_data[0])) {
        Spell_C_CancelSpellByCaster(pet);
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && changed && player->m_unit->m_unitFieldHealth <= 0) {
        if (s_pets.m_count && s_pets.m_data[0]) {
            FrameScript_SignalEvent(SCRIPT_EVENT_RAISED_AS_GHOUL, nullptr);
        } else {
            CGGameUI::UpdateActivePlayer();
        }
    }
}

// VERIFY: bb0a37d3 auto
// OFFSET: 0x005D4620
void CGPetInfo::PetAttackTarget(const WOWGUID& target, int32_t a2) {
    uint32_t action = 0x7000002;
    CGPetInfo::SendPetAction(action, target, a2);
}

// VERIFY: a7d77109 manual
// OFFSET: 0x005D36A0
int32_t Packet_SMSG_PET_UPDATE_COMBO_POINTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID pet;
    SmartGUID target;
    uint8_t points;
    *msg >> pet;
    *msg >> target;
    msg->Get(points);

    if (pet == CGPetInfo::GetPet(0)) {
        CGPetInfo::SetComboPoints(target, points);
    }

    return 1;
}

// VERIFY: 8d48f08a manual
// OFFSET: 0x005D30A0
void CGPetInfo::SetIsAttackingTarget(int32_t attacking) {
    if (attacking) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_ATTACK_START, nullptr);
        CGPetInfo::m_attackingTarget = attacking;
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_ATTACK_STOP, nullptr);
        CGPetInfo::m_attackingTarget = 0;
    }
}

// VERIFY: 62a49fb5 auto
// OFFSET: 0x005D2F90
void CGPetInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_PET_SPELLS_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_PET_LEARNED_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_PET_UNLEARNED_SPELLS);
    ClientServices::ClearMessageHandler(SMSG_PET_MODE);
    ClientServices::ClearMessageHandler(SMSG_PET_ACTION_FEEDBACK);
    ClientServices::ClearMessageHandler(SMSG_PET_BROKEN);
    ClientServices::ClearMessageHandler(SMSG_PET_RENAMEABLE);
    ClientServices::ClearMessageHandler(SMSG_PET_UPDATE_COMBO_POINTS);
    ClientServices::ClearMessageHandler(SMSG_PET_GUIDS);
}
