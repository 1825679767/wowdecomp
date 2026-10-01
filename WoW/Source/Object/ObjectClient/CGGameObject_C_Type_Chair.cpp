#include "CGGameObject_C_Type_Chair.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// VERIFY: a7465a7c manual
// OFFSET: 0x0070C6A0
CGGameObject_C_Type_Chair::CGGameObject_C_Type_Chair(CGGameObject_C* owner) : CGGameObject_C_TypeBase(owner) {
    this->m_interactDistance = 3.0f;
}

// VERIFY: b2eee098 auto
// OFFSET: 0x00710240
uint32_t CGGameObject_C_Type_Chair::GetNumSlots() const {
    uint32_t slotsIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 11);
    GameObjectStats* slotsStats = this->m_owner->m_stats;
    uint32_t slots = (slotsStats && slotsIndex < 24) ? slotsStats->m_propValue[slotsIndex] : 0;

    return slots;
}

// VERIFY: 9df5628b auto
// OFFSET: 0x0070C700
void CGGameObject_C_Type_Chair::PostInitWithStats() {
    uint32_t count = this->GetNumSlots();

    if (!this->m_owner->GetTransportGUID()) {
        GenerateChairPoints(this->m_owner->GetMatrix(), count, this->m_slots, 0.0f);
    } else {
        C44Matrix matrix;
        matrix.Translate(this->m_owner->GetRawPosition());
        C4Quaternion rotation(*reinterpret_cast<const C4QuaternionCompressed*>(&this->m_owner->m_passenger.m_facing));
        matrix.Rotate(rotation);
        matrix.Scale(this->m_owner->GetScale());
        GenerateChairPoints(matrix, count, this->m_slots, 0.0f);
    }
}

// VERIFY: 131cf891 manual
// OFFSET: 0x00711CE0
bool CGGameObject_C_Type_Chair::CanUse() const {
    if (!CGGameObject_C_TypeBase::CanUse()) {
        return false;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint8_t standState = player->m_unit->m_unitFieldBytes1[0];

    if (standState < 4 || standState > 6) {
        return true;
    }

    uint32_t count = this->GetNumSlots();

    while (count) {
        count--;
        C3Vector slot = this->m_owner->m_passenger.GetPosition(this->m_slots[count]);
        C3Vector position = player->GetPosition();
        float dx = position.x - slot.x;
        float dy = position.y - slot.y;
        float dz = position.z - slot.z;

        if (dx * dx + dz * dz + dy * dy < 0.16666667f) {
            return false;
        }
    }

    return true;
}
