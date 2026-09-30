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
