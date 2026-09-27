#include "Object_C.h"

// INLINED
CHashKeyGUID::CHashKeyGUID() {
}

// INLINED
CHashKeyGUID::CHashKeyGUID(WOWGUID guid) {
    this->m_guid = guid;
}

// INLINED
bool CHashKeyGUID::operator==(const CHashKeyGUID& key) {
    return this->m_guid == key.m_guid;
}
