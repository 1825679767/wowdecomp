#include "CGGameObject_C_Type_Chair.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"

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
