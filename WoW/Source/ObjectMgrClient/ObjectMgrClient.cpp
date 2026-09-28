#include <cstdint>
#include "ObjectMgrClient.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Net/NetClient/NetClient.h"

// STATIC: 0x00DD2008
static thread_local ObjectMgr* s_curMgr;

// STATIC: 0x00B4B014
static ObjectMgr* s_savedObjMgr;

// VERIFY: d1ddc062 manual
// OFFSET: 0x004D36C0
void NetClient::PushObjMgr(ClntObjMgr* mgr) {
    if (s_savedObjMgr) {
        return;
    }

    ObjectMgr* current = s_curMgr;

    if (reinterpret_cast<ObjectMgr*>(mgr) != current) {
        s_savedObjMgr = current;
        s_curMgr = reinterpret_cast<ObjectMgr*>(mgr);
    }
}

// VERIFY: a15d9aa7 manual
// OFFSET: 0x004D3700
void NetClient::PopObjMgr() {
    if (s_savedObjMgr) {
        s_curMgr = s_savedObjMgr;
        s_savedObjMgr = nullptr;
    }
}

// OFFSET: 0x004D3750
int32_t ClntObjMgrIsValid(int32_t a1) {
    ObjectMgr* mgr = s_curMgr;
    return mgr && SMemIsValidPointer(mgr, sizeof(ObjectMgr), a1);
}

// VERIFY: 562c7dc8 manual
// OFFSET: 0x004D3790
uint64_t ClntObjMgrGetActivePlayer() {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr) {
        return 0;
    }

    return static_cast<uint64_t>(mgr->m_playerGuid.m_guidHigh) << 32 | mgr->m_playerGuid.m_guidLow;
}

// VERIFY: 186e4107 manual
// OFFSET: 0x004D4DB0
CGObject_C* ClntObjMgrObjectPtr(WOWGUID guid, TypeMask mask, const char* fileName, int32_t lineNumber) {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr || !guid) {
        return nullptr;
    }

    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid));

    if (object && !(object->m_obj->m_type & mask)) {
        return nullptr;
    }

    return object;
}
