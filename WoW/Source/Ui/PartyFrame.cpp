#include "PartyFrame.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00BD1948
uint64_t CGPartyInfo::m_members[4];

// VERIFY: 5264a053 auto
// OFFSET: 0x0052BC50
uint64_t GetPartyMemberGUID(uint32_t index) {
    if (index >= 4) {
        return 0;
    }

    return CGPartyInfo::m_members[index];
}

// VERIFY: ad92fae3 auto
// OFFSET: 0x0052BD10
uint32_t CGPartyInfo::GetNumPartyMembers() {
    uint32_t count = 0;

    for (uint32_t i = 0; i < sizeof(CGPartyInfo::m_members) / sizeof(CGPartyInfo::m_members[0]); i++) {
        if (CGPartyInfo::m_members[i]) {
            count++;
        }
    }

    return count;
}

// VERIFY: 8d652a72 manual
// OFFSET: 0x0052C6E0
uint64_t GetPartyPetGUID(uint32_t index) {
    if (index >= 4 || !CGPartyInfo::m_members[index]) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[index], TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit) {
        const WGUID& pet = unit->m_unit->m_unitFieldCharm.m_guidLow || unit->m_unit->m_unitFieldCharm.m_guidHigh ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
        return static_cast<uint64_t>(pet.m_guidHigh) << 32 | pet.m_guidLow;
    }

    if (unk_BD0D10[index].m_flags & 0x1) {
        return static_cast<uint64_t>(unk_BD0D10[index].m_petGuid.m_guidHigh) << 32 | unk_BD0D10[index].m_petGuid.m_guidLow;
    }

    return 0;
}
