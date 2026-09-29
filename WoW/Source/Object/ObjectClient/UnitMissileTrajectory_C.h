#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITMISSILETRAJECTORY_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITMISSILETRAJECTORY_C_H

#include <cstddef>
#include <cstdint>

class CGUnit_C;
class CMissileCollision;

class CMissileCollisionHeap {
    public:
    /* 0000 */ uint32_t m_heapId;

    CMissileCollision* New();
};

class CMissileCollision {
    public:
    static CMissileCollisionHeap* s_pool;
    static CMissileCollision* s_head;
    static CMissileCollision* s_tail;

    static void InitSystem();
    static CMissileCollision* Alloc();
    static void RemoveUnitFromSystem(CGUnit_C* unit);
    static void MaybeAddUnitToSystem(CGUnit_C* unit);

    /* 0000 */ uint32_t m_memHandle;
    /* 0004 */ CGUnit_C* m_units [16];
    /* 0044 */ float unk_0044 [8];
    /* 0064 */ uint32_t unk_0064 [4];
    /* 0074 */ CMissileCollision* m_prev;
    /* 0078 */ CMissileCollision* m_next;

    CMissileCollision();
    void Free();
};

#endif
