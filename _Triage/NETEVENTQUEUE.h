#ifndef _TRIAGE_NETEVENTQUEUE_H
#define _TRIAGE_NETEVENTQUEUE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"

struct NETEVENTQUEUENODE;
class NetClient;

struct NETEVENTQUEUENODE {
    /* 0000 */ uint8_t m_eventId[4];
    /* 0004 */ uint32_t m_timeReceived;
    /* 0008 */ void* m_data;
    /* 000C */ uint32_t m_dataSize;
};

class NETEVENTQUEUE {
    public:
    /* 0000 */ NetClient* m_client;
    /* 0004 */ SCritSect m_critSect;
    /* 002C */ TSList<NETEVENTQUEUENODE> m_eventQueue;
};

#endif
