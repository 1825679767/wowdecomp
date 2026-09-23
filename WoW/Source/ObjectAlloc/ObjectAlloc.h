#ifndef WOW_SOURCE_OBJECTALLOC_OBJECTALLOC_H
#define WOW_SOURCE_OBJECTALLOC_OBJECTALLOC_H

#include <cstddef>
#include <cstdint>
#include "Storm/H/stpl.h"

struct CObjectHeap;

class CObjectHeapList {
    public:
    /* 0000 */ TSGrowableArray<CObjectHeap> m_heaps;
    /* 0010 */ uint32_t m_objSize;
    /* 0014 */ uint32_t m_objsPerBlock;
    /* 0018 */ uint32_t m_numFullHeaps;
    /* 001C */ uint32_t m_hasEmptyHeaps;
    /* 0020 */ uint32_t m_uint20;
    /* 0024 */ uint32_t m_fullestHeap;
    /* 0028 */ char m_heapName [80];
    /* 0078 */ uint32_t m_uint78;
    /* 007C */ uint32_t m_uint7C;
    /* 0080 */ uint32_t m_uint80;
    /* 0084 */ uint8_t m_freeEmptyHeaps;
    /* 0085 */ uint8_t unk_0085[3];
};

#endif
