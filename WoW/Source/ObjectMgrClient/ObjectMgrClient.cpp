#include <cstdint>
#include "ObjectMgrClient.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DD2008
static thread_local ObjectMgr* s_curMgr;

// OFFSET: 0x004D3750
int32_t ClntObjMgrIsValid(int32_t a1) {
    ObjectMgr* mgr = s_curMgr;
    return mgr && SMemIsValidPointer(mgr, sizeof(ObjectMgr), a1);
}
