#include "Engine/Source/Base/RefCount.h"

// INLINED
TRefCnt::TRefCnt() {
    this->m_refcnt = 0;
}

// VERIFY: 0b4d6aef manual
// OFFSET: 0x00407920
void TRefCnt::DeleteSelf() {
    delete this;
}

// INLINED
TRefCnt::~TRefCnt() {
}
