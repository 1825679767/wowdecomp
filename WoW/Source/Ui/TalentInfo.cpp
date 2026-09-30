#include "TalentInfo.h"

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
