#include "UnitMissileTrajectory_C.h"
#include <new>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

// STATIC: 0x00CA0A98
CMapEntity* CGUnit_C::s_trajectoryWorldObjects[2];

// STATIC: 0x00CA0AA0
CM2Model* CGUnit_C::s_trajectoryModels[2];

// STATIC: 0x00CA0AC8
C3Vector* CGUnit_C::s_trajectoryPoints;

// STATIC: 0x00CA0AD8
CMissileCollisionHeap* CMissileCollision::s_pool;

// STATIC: 0x00CA0ADC
CMissileCollision* CMissileCollision::s_head;

// STATIC: 0x00CA0AE0
CMissileCollision* CMissileCollision::s_tail;

// INLINED
inline CMissileCollision::CMissileCollision() {
    for (int32_t i = 0; i < 8; i++) {
        this->unk_0044[i] = 0.0f;
    }
}

// VERIFY: dba3ae87 manual
// OFFSET: 0x006FC0F0
void CMissileCollision::RemoveUnitFromSystem(CGUnit_C* unit) {
    for (CMissileCollision* collision = CMissileCollision::s_head; collision; collision = collision->m_next) {
        uint32_t i = 0;

        while (collision->m_units[i]) {
            if (collision->m_units[i] == unit) {
                do {
                    collision->m_units[i] = collision->m_units[i + 1];
                    i++;
                } while (collision->m_units[i]);

                break;
            }

            i++;
        }
    }
}

// INLINED
inline void CMissileCollision::InitSystem() {
    delete CMissileCollision::s_pool;

    CMissileCollisionHeap* pool = static_cast<CMissileCollisionHeap*>(SMemAlloc(sizeof(CMissileCollisionHeap), __FILE__, __LINE__, 0));

    if (pool) {
        pool->m_heapId = ObjectAllocAddHeap(sizeof(CMissileCollision), 8, "Missile Collision", true);
        CMissileCollision::s_tail = nullptr;
        CMissileCollision::s_head = nullptr;
        CMissileCollision::s_pool = pool;
    } else {
        CMissileCollision::s_pool = nullptr;
        CMissileCollision::s_tail = nullptr;
        CMissileCollision::s_head = nullptr;
    }
}

// VERIFY: 9a79f0ab manual
// OFFSET: 0x006FC770
CMissileCollision* CMissileCollisionHeap::New() {
    uint32_t memHandle;
    void* memory = nullptr;

    if (!ObjectAlloc(this->m_heapId, &memHandle, &memory, false)) {
        return nullptr;
    }

    CMissileCollision* collision = memory ? new (memory) CMissileCollision() : nullptr;
    collision->m_memHandle = memHandle;

    return collision;
}

// VERIFY: 4b422f42 manual
// OFFSET: 0x006FC8E0
void CGUnit_C::InitMissileTrajectorySystem() {
    C3Vector* points = static_cast<C3Vector*>(SMemAlloc(2400, __FILE__, __LINE__, 0));
    CGUnit_C::s_trajectoryModels[0] = nullptr;
    CGUnit_C::s_trajectoryWorldObjects[0] = nullptr;
    CGUnit_C::s_trajectoryModels[1] = nullptr;
    CGUnit_C::s_trajectoryWorldObjects[1] = nullptr;
    CGUnit_C::s_trajectoryPoints = points;
    CMissileCollision::InitSystem();
}

// VERIFY: 23f03795 auto
// OFFSET: 0x006FC950
void CMissileCollision::Free() {
    if (this == CMissileCollision::s_head) {
        CMissileCollision::s_head = this->m_next;

        if (CMissileCollision::s_head) {
            CMissileCollision::s_head->m_prev = nullptr;
        }
    } else if (this == CMissileCollision::s_tail) {
        CMissileCollision::s_tail = this->m_prev;

        if (this->m_prev) {
            this->m_prev->m_next = nullptr;
        }
    } else {
        this->m_prev->m_next = this->m_next;
        this->m_next->m_prev = this->m_prev;
    }

    ObjectFree(CMissileCollision::s_pool->m_heapId, this->m_memHandle);
}

// VERIFY: 0abf6409 auto
// OFFSET: 0x006FC920
CMissileCollision* CMissileCollision::Alloc() {
    CMissileCollision* collision = CMissileCollision::s_pool->New();
    CMissileCollision* head = CMissileCollision::s_head;
    collision->m_prev = nullptr;
    CMissileCollision::s_head = collision;

    if (head) {
        collision->m_next = head;
        head->m_prev = collision;
    } else {
        collision->m_next = nullptr;
        CMissileCollision::s_tail = collision;
    }

    return collision;
}
