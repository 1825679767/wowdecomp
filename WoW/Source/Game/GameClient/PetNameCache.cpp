#include "PetNameCache.h"

// INLINED
PetNameCache::PetNameCache() {
    this->m_petNumber = 0;
    this->m_name[0] = '\0';
    this->m_declinedNames = nullptr;
    this->m_timestamp = 0;
}

// INLINED
PetNameCache::~PetNameCache() {
    delete this->m_declinedNames;
}
