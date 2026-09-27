#ifndef _TRIAGE_WOWMOUSE_H
#define _TRIAGE_WOWMOUSE_H

#include <cstddef>
#include <cstdint>

class WowMouse {
    public:
    /* 0004 */ void* m_hidManager;

    virtual ~WowMouse();

    WowMouse();
};

#endif
