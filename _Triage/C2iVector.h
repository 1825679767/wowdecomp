#ifndef _TRIAGE_C2IVECTOR_H
#define _TRIAGE_C2IVECTOR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C2iVector.h"

class CGxMonitorMode {
    public:
    /* 0000 */ C2iVector m_size;
    /* 0008 */ uint32_t m_bpp;
    /* 000C */ uint32_t m_refreshRate;
};

#endif
