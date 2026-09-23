#ifndef _TRIAGE_CFG_CATEGORIESREC_H
#define _TRIAGE_CFG_CATEGORIESREC_H

#include <cstddef>
#include <cstdint>

class Cfg_CategoriesRec {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_localeMask;
    /* 0008 */ int32_t m_createCharsetMask;
    /* 000C */ int32_t m_flags;
    /* 0010 */ const char* m_name;
};

#endif
