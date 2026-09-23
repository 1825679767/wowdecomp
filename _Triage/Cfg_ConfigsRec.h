#ifndef _TRIAGE_CFG_CONFIGSREC_H
#define _TRIAGE_CFG_CONFIGSREC_H

#include <cstddef>
#include <cstdint>

class Cfg_ConfigsRec {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_realmType;
    /* 0008 */ int32_t m_playerKillingAllowed;
    /* 000C */ int32_t m_roleplaying;
};

#endif
