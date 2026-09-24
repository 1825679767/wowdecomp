#ifndef ENGINE_SOURCE_BASE_REFCOUNT_H
#define ENGINE_SOURCE_BASE_REFCOUNT_H

#include <cstdint>

class TRefCnt {
    public:
    /* 0004 */ uint32_t m_refcnt;

    TRefCnt();
    virtual void DeleteSelf();
    virtual ~TRefCnt();
};

// INLINED
inline TRefCnt::TRefCnt() {
    this->m_refcnt = 0;
}

// VERIFY: 9e4f100c manual
// OFFSET: 0x00407920
inline void TRefCnt::DeleteSelf() {
    delete this;
}

// INLINED
inline TRefCnt::~TRefCnt() {
}

#endif
