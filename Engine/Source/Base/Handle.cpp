#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/Handle.h"
#include "Storm/H/storm.h"

// VERIFY: 436d38d9 manual
// OFFSET: 0x0047BF30
void HandleClose(HOBJECT handle) {
    CHandleObject* object = HandleDereference(handle);

    if (object->m_refcount-- == 1) {
        delete object;
    }
}

// VERIFY: 1752397d auto
// OFFSET: 0x0047BF50
HOBJECT HandleCreate(CHandleObject* ptr, const char* handleName) {
    if (!ptr) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }

    ptr->m_refcount++;
    return reinterpret_cast<HOBJECT>(ptr);
}

// INLINED
CHandleObject* HandleDereference(HOBJECT handle) {
    return reinterpret_cast<CHandleObject*>(handle);
}

// VERIFY: 1d1aba75 manual
// OFFSET: 0x0047BF70
HOBJECT HandleDuplicate(HOBJECT handle) {
    if (handle) {
        HandleDereference(handle)->m_refcount++;
    }

    return handle;
}

void HandleDestroy() {
}

void HandleInitialize() {
}
