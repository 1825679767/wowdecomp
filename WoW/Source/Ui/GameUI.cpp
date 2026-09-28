#include "GameUI.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/World.h"
#include "_Triage/GxRestart.h"

// STATIC: 0x00BD07A0
uint64_t CGGameUI::m_currentObjectTrack;

// STATIC: 0x00BD07A8
uint64_t CGGameUI::m_interactTarget;

// STATIC: 0x00BD07B0
uint64_t CGGameUI::m_lockedTarget;

// STATIC: 0x00BD07C0
uint64_t CGGameUI::m_lastEnemyTarget;

// STATIC: 0x00BD07D0
uint64_t CGGameUI::unk_BD07D0;

// STATIC: 0x00BD0800
int32_t CGGameUI::m_inMovie;

// STATIC: 0x00BD0880
int32_t CGGameUI::m_initialized;

// STATIC: 0x00BD0D10
CGPartyMemberState unk_BD0D10[4];

// VERIFY: 6c643fe3 manual
// OFFSET: 0x00512900
void OnGxRestartClearWorld() {
    if (CGWorldFrame::s_currentWorldFrame) {
        CWorldScene::ClearObjectFadeoutList();
    }

    UpdateAllPortraits();
}

// VERIFY: 0b5b7966 auto
// OFFSET: 0x00512A60
uint64_t CGGameUI::GetPartyMember(uint32_t index) {
    return GetPartyMemberGUID(index);
}

// VERIFY: e362e3cb auto
// OFFSET: 0x00512A70
uint64_t CGGameUI::GetPartyMemberPet(uint32_t index) {
    return GetPartyPetGUID(index);
}

// VERIFY: 7c3fe02d auto
// OFFSET: 0x00512A80
uint64_t GetRaidMemberGUID(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers || !CGRaidInfo::m_members[index]) {
        return 0;
    }

    return static_cast<uint64_t>(CGRaidInfo::m_members[index]->m_guid.m_guidHigh) << 32 | CGRaidInfo::m_members[index]->m_guid.m_guidLow;
}

// VERIFY: 4073e0e6 manual
// OFFSET: 0x00518CE0
uint64_t CGGameUI::GetRaidMemberPet(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers) {
        return 0;
    }

    CGRaidMember* member = CGRaidInfo::m_members[index];

    if (!member) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(static_cast<uint64_t>(member->m_guid.m_guidHigh) << 32 | member->m_guid.m_guidLow, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return static_cast<uint64_t>(member->m_petGuid.m_guidHigh) << 32 | member->m_petGuid.m_guidLow;
    }

    const WGUID& pet = unit->m_unit->m_unitFieldCharm.m_guidLow || unit->m_unit->m_unitFieldCharm.m_guidHigh ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    return static_cast<uint64_t>(pet.m_guidHigh) << 32 | pet.m_guidLow;
}
