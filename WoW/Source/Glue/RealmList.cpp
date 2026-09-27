#include "RealmList.h"
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00B6B09C
RealmSortCriterion CRealmList::m_sortCriteria[4];

// STATIC: 0x00B6B0C4
TSFixedArray<RealmCategory*> CRealmList::m_categories;

// VERIFY: 98bc502a manual
// OFFSET: 0x004DF580
void CRealmList::Initialize() {
    uint32_t count = g_cfg_CategoriesDB.m_numRecords;
    CRealmList::m_categories.SetCount(count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        CRealmList::m_categories[i] = new (SMemAlloc(sizeof(RealmCategory), __FILE__, __LINE__, 0)) RealmCategory;
        CRealmList::m_categories[i]->m_category = i >= 0 && i < g_cfg_CategoriesDB.m_numRecords ? &g_cfg_CategoriesDB.m_records[i] : nullptr;
        CRealmList::m_categories[i]->unk_0014 = 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        CRealmList::m_sortCriteria[i].m_criterion = i;
        CRealmList::m_sortCriteria[i].m_reverse = 0;
    }
}
