#include "LFGInfo.h"

// STATIC: 0x00A394B0
static const uint8_t s_classRoles[11] = { 0x0B, 0x0F, 0x09, 0x09, 0x0D, 0x0B, 0x0D, 0x09, 0x09, 0x00, 0x0F };

// VERIFY: 7da483d6 auto
// OFFSET: 0x00552940
uint8_t CGLookingForGroup::GetClassRoles(uint32_t classID) {
    return s_classRoles[classID - 1];
}
