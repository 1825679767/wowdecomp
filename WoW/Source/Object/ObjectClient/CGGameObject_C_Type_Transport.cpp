#include "CGGameObject_C_Type_Transport.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/TransportAnimationRec.h"
#include "WoW/Source/DB/Rec/TransportRotationRec.h"

// VERIFY: 0bbb79ee manual
// OFFSET: 0x0070C8C0
static int32_t FindTransportAnimationIndex(int32_t transportId) {
    int32_t high = g_transportAnimationDB.m_numRecords;
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;
        const TransportAnimationRec* first = (0 >= g_transportAnimationDB.m_numRecords) ? nullptr : &g_transportAnimationDB.m_records[0];

        if (first[mid].m_transportID >= transportId) {
            high = mid;
        } else {
            low = mid + 1;
        }
    } while (low < high);

    const TransportAnimationRec* first = (0 >= g_transportAnimationDB.m_numRecords) ? nullptr : &g_transportAnimationDB.m_records[0];

    if (low < g_transportAnimationDB.m_numRecords && first[low].m_transportID == transportId) {
        return low;
    }

    return -1;
}

// VERIFY: 8c4056b2 manual
// OFFSET: 0x0070C930
static int32_t FindTransportRotationIndex(int32_t transportId) {
    int32_t high = g_transportRotationDB.m_numRecords;
    int32_t low = 0;

    if (high <= 0) {
        return -1;
    }

    do {
        int32_t mid = (high + low) / 2;
        const TransportRotationRec* first = (0 >= g_transportRotationDB.m_numRecords) ? nullptr : &g_transportRotationDB.m_records[0];

        if (first[mid].m_gameObjectsID >= transportId) {
            high = mid;
        } else {
            low = mid + 1;
        }
    } while (low < high);

    const TransportRotationRec* first = (0 >= g_transportRotationDB.m_numRecords) ? nullptr : &g_transportRotationDB.m_records[0];

    if (low < g_transportRotationDB.m_numRecords && first[low].m_gameObjectsID == transportId) {
        return low;
    }

    return -1;
}

// VERIFY: e9e508da manual
// OFFSET: 0x00713820
CGGameObject_C_Type_Transport::CGGameObject_C_Type_Transport(CGGameObject_C* owner) : CGGameObject_C_Type_TransportBase(owner), m_frames(nullptr), m_frameCount(0), unk_0048(0), m_rotations(nullptr), m_rotationCount(0), unk_0054(0), unk_0058(506), m_stateChangeSequenceTime(0) {
    int32_t index = FindTransportAnimationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_frames = reinterpret_cast<TransportFrame*>((index < 0 || index >= g_transportAnimationDB.m_numRecords) ? nullptr : &g_transportAnimationDB.m_records[index]);
        this->m_frameCount = 1;

        for (int32_t i = index + 1; i < g_transportAnimationDB.m_numRecords; i++) {
            const TransportAnimationRec* rec = (i < 0 || i >= g_transportAnimationDB.m_numRecords) ? nullptr : &g_transportAnimationDB.m_records[i];

            if (rec->m_transportID != owner->m_obj->m_entryID) {
                break;
            }

            this->m_frameCount++;
        }
    }

    index = FindTransportRotationIndex(owner->m_obj->m_entryID);

    if (index != -1) {
        this->m_rotations = (index < 0 || index >= g_transportRotationDB.m_numRecords) ? nullptr : &g_transportRotationDB.m_records[index];
        this->m_rotationCount = 1;

        for (int32_t i = index + 1; i < g_transportRotationDB.m_numRecords; i++) {
            const TransportRotationRec* rec = (i < 0 || i >= g_transportRotationDB.m_numRecords) ? nullptr : &g_transportRotationDB.m_records[i];

            if (rec->m_gameObjectsID != owner->m_obj->m_entryID) {
                break;
            }

            this->m_rotationCount++;
        }
    }

    this->InitializeMovement();
}

// VERIFY: 9cb18f7c auto
// OFFSET: 0x00710640
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(int32_t state, uint32_t time) const {
    uint32_t count = this->m_frameCount;

    if (!count) {
        return time;
    }

    uint32_t period = this->m_owner->m_gameObject->m_level;

    if (!period) {
        return time % this->m_frames[count - 1].m_time;
    }

    uint32_t elapsed = time - this->m_stateChangeTime;
    int32_t currentState = this->unk_0038;
    uint32_t result;

    if (state == currentState) {
        result = this->m_stateChangeSequenceTime + elapsed;
    } else if (elapsed < this->m_stateChangeSequenceTime) {
        result = this->m_stateChangeSequenceTime - elapsed;
    } else {
        result = 0;
    }

    if (currentState) {
        result += period;

        if (result >= this->m_frames[count - 1].m_time) {
            return 0;
        }
    } else if (result > period) {
        return period;
    }

    return result;
}

// VERIFY: 4acdedb5 auto
// OFFSET: 0x00713990
uint32_t CGGameObject_C_Type_Transport::GetSequenceTimeFromRealTime(uint32_t time) const {
    return this->GetSequenceTimeFromRealTime(this->m_owner->m_gameObject->m_state, time);
}

// VERIFY: 15b1af82 manual
// OFFSET: 0x00712010
void CGGameObject_C_Type_Transport::Reenable() {
    CGGameObject_C_Type_TransportBase::Reenable();
    this->InitializeMovement();
}

// VERIFY: 9abb4c2a auto
// OFFSET: 0x0070B5C0
void CGGameObject_C_Type_Transport::PostInitWithStats() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    if (this->m_worldObject) {
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }
}

// VERIFY: 1347bf69 auto
// OFFSET: 0x0070B630
void CGGameObject_C_Type_Transport::PostReenable() {
    this->unk_0058 = 506;
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    if (this->m_worldObject) {
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
    }
}
