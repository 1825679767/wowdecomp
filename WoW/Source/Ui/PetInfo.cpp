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

// STATIC: 0x00C234E4
uint8_t CGPetInfo::m_comboPoints;

// STATIC: 0x00C234E8
WOWGUID CGPetInfo::m_comboTarget;

// STATIC: 0x00C234F8
static TSFixedArray<WOWGUID> s_pets;

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

// STATIC: 0x00C23508
uint32_t CGPetInfo::m_actions[10];

// STATIC: 0x00C234C8
uint32_t CGPetInfo::m_petMode;

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

// STATIC: 0x00A1A860
static const char s_petModeTokens[3][32] = { "PASSIVE", "DEFENSIVE", "AGGRESSIVE" };

// STATIC: 0x00A1A8C0
static const char s_petOrderTokens[4][32] = { "WAIT", "FOLLOW", "ATTACK", "DISMISS" };

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
