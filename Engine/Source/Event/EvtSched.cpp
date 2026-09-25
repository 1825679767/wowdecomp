#include <cstdint>
#include "Engine/Source/Event/EvtContext.h"
#include "Engine/Source/Event/EvtSched.h"

// VERIFY: f0d6cd9f auto
// OFFSET: 0x0047DC90
int32_t IEvtSchedulerIsContextInteractive(EvtContext* context) {
    return (context->m_schedFlags >> 1) & 1;
}
