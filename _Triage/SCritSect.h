#ifndef _TRIAGE_SCRITSECT_H
#define _TRIAGE_SCRITSECT_H

#include <cstddef>
#include <cstdint>

class SCritSect {
    public:
    /* 0000 */ uint8_t m_critsect [40];
};

#endif
