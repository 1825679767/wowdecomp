#include "DanceCache.h"

// INLINED
DanceCache::DanceCache() {
    this->m_id = 0;
    this->m_guid = WOWGUID();
    this->m_name[0] = '\0';
    this->m_data = nullptr;
    this->m_dataCount = 0;
    this->unk_0098 = 0;
}

// INLINED
DanceCache::~DanceCache() {
    delete[] this->m_data;
}
