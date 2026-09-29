#include "CGGameObject_C_Type_BarberChair.h"

// VERIFY: 7687419a manual
// OFFSET: 0x0070C7F0
CGGameObject_C_Type_BarberChair::CGGameObject_C_Type_BarberChair(CGGameObject_C* owner) : CGGameObject_C_Type_Chair(owner) {
}

// VERIFY: 4885b81c auto
// OFFSET: 0x008C8DE0
uint32_t CGGameObject_C_Type_BarberChair::GetNumSlots() const {
    return 1;
}
