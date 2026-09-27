#include <cstddef>
#include <cstdint>
#include "WoW/Source/SoundInterface/SoundInterface2Water.h"
#include "Engine/Source/Event/Event.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: 0x00B4AF5C
static SI2WaterSound s_nearestWaterSounds[1];

// VERIFY: cf194119 auto
// OFFSET: 0x004D2750
void SI2::WaterShutdown() {
    SI2::StopSound(&s_nearestWaterSounds[0].m_object, 0, -1.0f, 1);
    EventUnregister(EVENT_ID_IDLE, SI2::HandleWaterSounds);
}
