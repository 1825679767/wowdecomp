#ifndef ENGINE_SOURCE_BASE_HANDLE_H
#define ENGINE_SOURCE_BASE_HANDLE_H

#include <cstdint>
#include "Engine/Source/Base/Types.h"

class CHandleObject {
    public:
    /* 0004 */ int32_t m_refcount;

    virtual ~CHandleObject();
    virtual const char* GetObjectName();

    CHandleObject();
};

void HandleClose(HOBJECT handle);
HOBJECT HandleCreate(CHandleObject* ptr, const char* handleName);
CHandleObject* HandleDereference(HOBJECT handle);
void HandleDestroy();
HOBJECT HandleDuplicate(HOBJECT handle);
void HandleInitialize();

// INLINED
inline CHandleObject::CHandleObject() {
    this->m_refcount = 0;
}

// INLINED
inline CHandleObject::~CHandleObject() {
}

// INLINED
inline const char* CHandleObject::GetObjectName() {
    return nullptr;
}

#endif
