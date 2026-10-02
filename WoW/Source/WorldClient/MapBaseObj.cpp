#include "MapBaseObj.h"

// VERIFY: a9c26690 manual
// OFFSET: [0x007D71D0, 0x007D72A0]
CMapBaseObj::~CMapBaseObj() {
}

// VERIFY: ec60c430 manual
// OFFSET: 0x007D7260
CMapBaseObj::CMapBaseObj() {
    this->m_type = 1;
    this->m_refCount = 0;
    this->m_flags = static_cast<MapObjFlag>(0);
}
