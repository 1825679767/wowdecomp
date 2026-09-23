#ifndef ENGINE_SOURCE_EVENT_EVTTHREAD_H
#define ENGINE_SOURCE_EVENT_EVTTHREAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Event/EvtContextQueue.h"
#include "Storm/H/stpl.h"
#include "_Triage/SEvent.h"

class EvtThread;

class EvtThread : public TSLinkedNode<EvtThread> {
    public:
    /* 0008 */ uint32_t m_threadSlot;
    /* 000C */ uint32_t m_threadCount;
    /* 0010 */ uint32_t m_weightTotal;
    /* 0014 */ uint32_t m_weightAvg;
    /* 0018 */ uint32_t m_contextCount;
    /* 001C */ uint32_t m_rebalance;
    /* 0020 */ SEvent m_wakeEvent;
    /* 0024 */ EvtContextQueue m_contextQueue;
};

#endif
