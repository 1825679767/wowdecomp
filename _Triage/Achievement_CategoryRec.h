#ifndef _TRIAGE_ACHIEVEMENT_CATEGORYREC_H
#define _TRIAGE_ACHIEVEMENT_CATEGORYREC_H

#include <cstddef>
#include <cstdint>

class Achievement_CategoryRec {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parent;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_uiOrder;
};

#endif
