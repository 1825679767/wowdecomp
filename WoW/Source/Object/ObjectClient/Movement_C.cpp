#include "Movement_C.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "WoW/Source/Object/CMovementGlobals.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00ADB74C
static TSExplicitList<CGGameObject_C, offsetof(CGGameObject_C, m_transportLink)> s_transports;

// VERIFY: 15b28f8e auto
// OFFSET: 0x0074B730
void MovementAddTransport(CGGameObject_C* transport) {
    s_transports.LinkToTail(transport);
}

// VERIFY: 9a06aa21 manual
// OFFSET: 0x0074B340
int32_t MovementAddToTransport(CPassenger* passenger, WOWGUID transport, int32_t reason) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    return object->AddPassenger(passenger, reason);
}

// VERIFY: b62046a6 auto
// OFFSET: 0x0074B330
CMovementGlobals* MovementGetGlobals() {
    return ClntObjMgrGetMovementGlobals();
}

// VERIFY: 49dca93b manual
// OFFSET: 0x0074B590
float MovementGetTransportFacing(WOWGUID transport) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X", transport);
        return 0.0f;
    }

    return object->GetFacing();
}

// VERIFY: 87477b93 manual
// OFFSET: 0x0074B430
int32_t MovementGetTransportMtxSafe(WOWGUID transport, C44Matrix* matrix) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        *matrix = object->GetMatrix();
        return 1;
    }

    matrix->d3 = 1.0f;
    matrix->c2 = 1.0f;
    matrix->b1 = 1.0f;
    matrix->a0 = 1.0f;
    matrix->d2 = 0.0f;
    matrix->d1 = 0.0f;
    matrix->d0 = 0.0f;
    matrix->c3 = 0.0f;
    matrix->c1 = 0.0f;
    matrix->c0 = 0.0f;
    matrix->b3 = 0.0f;
    matrix->b2 = 0.0f;
    matrix->b0 = 0.0f;
    matrix->a3 = 0.0f;
    matrix->a2 = 0.0f;
    matrix->a1 = 0.0f;
    return 0;
}

// VERIFY: 513fa136 auto
// OFFSET: 0x0074B4C0
int32_t MovementGetTransportMtxX(WOWGUID transport, C44Matrix* matrix, WOWGUID owner, const char* file, int32_t line) {
    if (MovementGetTransportMtxSafe(transport, matrix)) {
        return 1;
    }

    DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X from %s(%d)", transport, file, line);
    return 0;
}

// VERIFY: 11a64fa7 manual
// OFFSET: 0x0074B510
void MovementGetTransportQuat(WOWGUID transport, C4Quaternion* quat) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        *quat = object->GetRotation();
        return;
    }

    DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X", transport);
    quat->x = 0.0f;
    quat->y = 0.0f;
    quat->z = 0.0f;
    quat->w = 1.0f;
}

// VERIFY: 884e851b manual
// OFFSET: 0x0074B5E0
int32_t MovementInsideTransport(WOWGUID transport, const C3Vector& point) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    return object->IsPointInside(point);
}

// VERIFY: e4c387f6 manual
// OFFSET: 0x0074B6E0
void MovementMoveTransports(uint32_t time, uint32_t elapsed) {
    for (CGGameObject_C* transport = s_transports.Head(); transport; transport = s_transports.Next(transport)) {
        transport->m_typeData->UpdateMovement(time, elapsed);
    }
}

// VERIFY: 4ae3b26d manual
// OFFSET: 0x0074B670
void MovementNotifyTransportUnitRemoved(WOWGUID passenger, WOWGUID transport) {
    uint32_t high = (uint32_t)(transport.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | (uint32_t)transport.m_guid))) {
        CGUnit_C* unit = (CGUnit_C*)ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__);

        if (unit && unit->m_vehicle) {
            unit->m_vehicle->UpdateFreeSeatMask();
        }
    }
}

// VERIFY: 06762f05 auto
// OFFSET: 0x005EEB70
void MovementNotifyZoneMgr(WOWGUID guid) {
}

// VERIFY: a22f2752 manual
// OFFSET: 0x0074B3F0
int32_t MovementObjIsTransport(WOWGUID guid) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    return object->IsTransport();
}

// VERIFY: abd26a7a auto
// OFFSET: 0x0074B750
void MovementRemoveTransport(CGGameObject_C* transport) {
    s_transports.UnlinkNode(transport);
}

// VERIFY: 5adebd9b auto
// OFFSET: 0x006E8F90
int32_t MovementGetLastUpdateTime(uint32_t* time) {
    CMovementGlobals* globals = MovementGetGlobals();

    if (!globals || !(globals->m_flags & 0x1)) {
        return 0;
    }

    *time = globals->m_lastUpdateTime;

    return 1;
}
