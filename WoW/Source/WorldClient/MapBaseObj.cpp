#include "MapBaseObj.h"

// VERIFY: ec60c430 manual
// OFFSET: 0x007D7260
CMapBaseObj::CMapBaseObj() {
    this->m_type = 1;
    this->m_refCount = 0;
    this->m_flags = static_cast<MapObjFlag>(0);
}
