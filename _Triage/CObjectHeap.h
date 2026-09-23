#ifndef _TRIAGE_COBJECTHEAP_H
#define _TRIAGE_COBJECTHEAP_H

#include <cstddef>
#include <cstdint>

class CObjectHeap {
    public:
    /* 0000 */ void* m_obj;
    /* 0004 */ uint32_t* m_indexStack;
    /* 0008 */ uint32_t m_allocated;
};

#endif
