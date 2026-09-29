#include "NameCache.h"

// INLINED
NameCache::NameCache() {
    this->m_name[0] = '\0';
    this->m_declinedNames = nullptr;
    this->m_realmName[0] = '\0';
    this->m_guid = WOWGUID();
}

// INLINED
NameCache::~NameCache() {
    delete this->m_declinedNames;
}
