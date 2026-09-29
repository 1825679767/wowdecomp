#include "PartyFrame.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00BD1948
WOWGUID CGPartyInfo::m_members[4];

// STATIC: 0x00BD0D08
CGPartyMemberState unk_BD0D08[4];

// VERIFY: e02ce9a8 auto
// OFFSET: 0x0052BC50
WOWGUID GetPartyMemberGUID(uint32_t index) {
    if (index >= 4) {
        return WOWGUID();
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

// VERIFY: 0146e892 manual
// OFFSET: 0x0052C6E0
WOWGUID GetPartyPetGUID(uint32_t index) {
    if (index >= 4 || !CGPartyInfo::m_members[index]) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPartyInfo::m_members[index], TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (unit) {
        const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
        return pet;
    }

    if (unk_BD0D08[index].m_flags & 0x1) {
        return unk_BD0D08[index].m_pet.m_guid;
    }

    return WOWGUID();
}
