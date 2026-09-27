#include "ComSatClient.h"
#include <new>
#include "Engine/Source/Storm/H/stpl.h"

// STATIC: 0x00D31AA0
COMSATDATA s_comsatData[32];

// STATIC: 0x00D37CA0
int32_t s_voiceActivation;

// STATIC: 0x00D37D04
int32_t s_voiceChatWasEnabled;

// STATIC: 0x00D37D44
static TSGrowableArray<ComSatEvent> s_comSatEvents;

// VERIFY: 4b73a2c5 manual
// OFFSET: 0x007DCA90
void AddComSatEvent(uint32_t type, float value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_floatValue = value;
        event->m_type = type;
        event->m_guid = guid;
    }
}

// VERIFY: ebc8d780 manual
// OFFSET: 0x007DCB10
void AddComSatEvent(uint32_t type, uint32_t value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_type = type;
        event->m_guid = guid;
        event->m_uintValue = value;
    }
}
