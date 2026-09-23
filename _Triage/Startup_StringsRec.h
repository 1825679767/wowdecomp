#ifndef _TRIAGE_STARTUP_STRINGSREC_H
#define _TRIAGE_STARTUP_STRINGSREC_H

#include <cstddef>
#include <cstdint>

class Startup_StringsRec {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_message;
};

#endif
