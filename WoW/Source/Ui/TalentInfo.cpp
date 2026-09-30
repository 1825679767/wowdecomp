#include "TalentInfo.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/PetInfo.h"

// STATIC: 0x00C21008
WOWGUID CGTalentInfo_Inspect::m_inspectTarget;

// STATIC: 0x00C20FF4
uint32_t CGTalentInfo::m_activeTalentGroup;

// STATIC: 0x00C21004
uint32_t CGTalentInfo_Inspect::m_activeTalentGroup;

// STATIC: 0x00C20FF8
uint32_t CGTalentInfo::m_numTalentGroups;

// STATIC: 0x00C21000
int32_t CGTalentInfo::m_petTalentTab;

// STATIC: 0x00C21018
TSFixedArray<TalentTabInfo*> CGTalentInfo::m_tabs;

// STATIC: 0x00C21024
TSFixedArray<TalentTabInfo*> CGTalentInfo::m_petTabs;

// STATIC: 0x00C21030
CGTalentGroup_C CGTalentInfo::m_talentGroups[2];

// STATIC: 0x00C210D8
CGTalentGroup_C CGTalentInfo::m_petTalentGroup;

// VERIFY: 05fa25fb auto
// OFFSET: 0x005C57C0
void CGTalentInfo_Inspect::EnterWorld() {
    CGTalentInfo_Inspect::m_inspectTarget = WOWGUID{ 0 };
}

// VERIFY: 1636395f manual
// OFFSET: 0x005C5BE0
int32_t CGTalentInfo::SumPreviewPointsSpent(int32_t pet) {
    int32_t total = 0;

    if (pet) {
        if (CGTalentInfo::m_petTalentTab && CGTalentInfo::m_petTabs.m_count) {
            for (uint32_t i = 0; i < CGTalentInfo::m_petTalentGroup.m_tabs.m_count; i++) {
                total += CGTalentInfo::m_petTalentGroup.m_tabs.m_data[i].m_previewPointsSpent;
            }
        }
    } else {
        for (uint32_t group = 0; group < CGTalentInfo::m_numTalentGroups; group++) {
            int32_t spent = 0;

            for (uint32_t i = 0; i < CGTalentInfo::m_talentGroups[group].m_tabs.m_count; i++) {
                spent += CGTalentInfo::m_talentGroups[group].m_tabs.m_data[i].m_previewPointsSpent;
            }

            total += spent;
        }
    }

    return total;
}

// VERIFY: b993cffb manual
// OFFSET: 0x005C70D0
void CGTalentInfo::RecalculatePreviewPoints(int32_t pet) {
    if (pet) {
        if (CGTalentInfo::m_petTalentTab && CGTalentInfo::m_petTabs.m_count) {
            CGTalentInfo::m_petTalentGroup.ResetAllPreviewTalentPoints();
        }
    } else {
        for (uint32_t group = 0; group < CGTalentInfo::m_numTalentGroups; group++) {
            CGTalentInfo::m_talentGroups[group].ResetAllPreviewTalentPoints();
        }
    }
}

// VERIFY: 9d9e1595 auto
// OFFSET: 0x005C57A0
uint32_t CGTalentInfo::GetActiveTalentGroup(int32_t pet) {
    return pet == 0 ? CGTalentInfo::m_activeTalentGroup : 0;
}

// VERIFY: 9f283c28 auto
// OFFSET: 0x005C5B30
TalentGroupTab* CGTalentGroup_C::GetTabFromID(int32_t tabId) {
    for (uint32_t i = 0; i < this->m_tabs.m_count; i++) {
        if (this->m_tabs.m_data[i].m_tabId == tabId) {
            return &this->m_tabs.m_data[i];
        }
    }

    return nullptr;
}

// VERIFY: 624e530c manual
// OFFSET: 0x005C5F70
uint32_t CGTalentInfo::GetUnspentTalentPoints(uint32_t index, int32_t pet) {
    uint32_t count;

    if (pet) {
        if (!CGTalentInfo::m_petTalentTab || !CGTalentInfo::m_petTabs.m_count) {
            return 0;
        }

        count = 1;
    } else {
        count = CGTalentInfo::m_numTalentGroups;
    }

    if (index >= count) {
        return 0;
    }

    if (pet) {
        return CGTalentInfo::m_petTalentGroup.m_unspentPoints;
    }

    CGTalentGroup_C* group = &CGTalentInfo::m_talentGroups[index];
    return group ? group->m_unspentPoints : 0;
}

// VERIFY: 73a4e27f auto
// OFFSET: 0x005C5FC0
CGTalentGroup_C* CGTalentInfo::GetTalentGroup(uint32_t index, int32_t pet) {
    uint32_t count;

    if (pet) {
        if (!CGTalentInfo::m_petTalentTab || !CGTalentInfo::m_petTabs.m_count) {
            return nullptr;
        }

        count = 1;
    } else {
        count = CGTalentInfo::m_numTalentGroups;
    }

    if (index >= count) {
        return nullptr;
    }

    return pet ? &CGTalentInfo::m_petTalentGroup : &CGTalentInfo::m_talentGroups[index];
}

// VERIFY: 02243f68 manual
// OFFSET: 0x005C60C0
int32_t TalentInfoIsSpellKnown(int32_t spellId, int32_t isInspect, int32_t isPet) {
    if (isInspect) {
        return isPet == 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    if (isPet) {
        unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            return 0;
        }
    }

    return unit->IsSpellKnown(spellId) || unit->IsSpellSuperceded(spellId);
}
