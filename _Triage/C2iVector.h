#ifndef _TRIAGE_C2IVECTOR_H
#define _TRIAGE_C2IVECTOR_H

#include <cstddef>
#include <cstdint>

class C2iVector {
    public:
    /* 0000 */ int32_t m_x;
    /* 0004 */ int32_t m_y;
};

class CGxMonitorMode {
    public:
    /* 0000 */ C2iVector m_size;
    /* 0008 */ uint32_t m_bpp;
    /* 000C */ uint32_t m_refreshRate;
};

#endif
