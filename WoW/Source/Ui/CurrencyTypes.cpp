#include "CurrencyTypes.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"

// STATIC: 0x00C1F44C
int32_t CGCurrencyTypes::m_listSize;

// STATIC: 0x00C1E960
CurrencyListEntry CGCurrencyTypes::m_list[95];

// STATIC: 0x00C1F248
CurrencyCategoryInfo CGCurrencyTypes::m_categories[32];

// STATIC: 0x00C1F448
int32_t CGCurrencyTypes::m_numCategories;

// VERIFY: 74b83f99 auto
// OFFSET: 0x005AFC40
int32_t CGCurrencyTypes::CurrencyCategorySortCallback(const void* a, const void* b) {
    const CurrencyCategoryInfo* categoryA = static_cast<const CurrencyCategoryInfo*>(a);
    const CurrencyCategoryInfo* categoryB = static_cast<const CurrencyCategoryInfo*>(b);

    if (!categoryA || !categoryB) {
        return 0;
    }

    int32_t result = (categoryA->m_flags & 1) - (categoryB->m_flags & 1);

    if (!result) {
        return SStrSortUTF8I(categoryA->m_name, categoryB->m_name, 0x7FFFFFFF);
    }

    return result;
}

// VERIFY: 7bd12f82 manual
// OFFSET: 0x005AFD90
void CGCurrencyTypes::InitializeGame() {
    int32_t numCategories = g_currencyCategoryDB.m_numRecords;
    CGCurrencyTypes::m_numCategories = g_currencyCategoryDB.m_numRecords;
    memset(CGCurrencyTypes::m_categories, 0, sizeof(CGCurrencyTypes::m_categories));

    for (int32_t i = 0; i < numCategories; i++) {
        CurrencyCategoryRec* rec = i < 0 || i >= numCategories ? nullptr : &g_currencyCategoryDB.m_records[i];

        CGCurrencyTypes::m_categories[i].m_id = rec->m_id;
        CGCurrencyTypes::m_categories[i].m_name = rec->m_name;
        CGCurrencyTypes::m_categories[i].m_flags = rec->m_flags;
        CGCurrencyTypes::m_categories[i].m_expanded = 1;
    }

    qsort(CGCurrencyTypes::m_categories, numCategories, sizeof(CurrencyCategoryInfo), CGCurrencyTypes::CurrencyCategorySortCallback);
    CGCurrencyTypes::m_listSize = 0;
    memset(CGCurrencyTypes::m_list, 0, sizeof(CGCurrencyTypes::m_list));
}
