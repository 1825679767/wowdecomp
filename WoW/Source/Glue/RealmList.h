#ifndef WOW_SOURCE_GLUE_REALMLIST_H
#define WOW_SOURCE_GLUE_REALMLIST_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class Cfg_CategoriesRec;

struct RealmCategory {
    /* 0000 */ Cfg_CategoriesRec* m_category;
    /* 0004 */ TSGrowableArray<uint32_t> m_realms;
    /* 0014 */ uint32_t unk_0014;
};

struct RealmSortCriterion {
    /* 0000 */ int32_t m_criterion;
    /* 0004 */ int32_t m_reverse;
};

class CRealmList {
    public:
    static RealmSortCriterion m_sortCriteria[4];
    static TSFixedArray<RealmCategory*> m_categories;

    static void Initialize();
    static void Shutdown();
    static void UpdateList();
};

void RealmListRegisterScriptFunctions();

#endif
