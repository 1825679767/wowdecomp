#include "CGGameObject_C_Type_TrapDoor.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/World.h"

// VERIFY: 84b1b349 auto
// OFFSET: 0x00713790
CGGameObject_C_Type_TrapDoor::CGGameObject_C_Type_TrapDoor(CGGameObject_C* owner) : CGGameObject_C_Type_TransportBase(owner) {
}

// VERIFY: 7c534dd2 auto
// OFFSET: 0x0070B580
void CGGameObject_C_Type_TrapDoor::PostReenable() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    if (this->m_worldObject) {
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
    }
}

// VERIFY: 70c8e6d1 auto
// OFFSET: 0x007137B0
void CGGameObject_C_Type_TrapDoor::UpdateMovement(uint32_t time, uint32_t elapsed) {
    if (!this->m_worldObject) {
        return;
    }

    if (!this->unk_0034 && World::ObjectIsLoaded(this->m_worldObject)) {
        this->unk_0034 = 1;
        int32_t moved;
        this->MovePassengers(&moved);
        return;
    }

    World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
}
