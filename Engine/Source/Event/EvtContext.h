#ifndef ENGINE_SOURCE_EVENT_EVTCONTEXT_H
#define ENGINE_SOURCE_EVENT_EVTCONTEXT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Event/EvtTimer.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"

class EvtContext;
struct EvtHandler;
struct EvtKeyDown;
struct EvtMessage;
struct EvtTimer;

template <class T>
class CSBasePriorityQueue {
    public:
    /* 0000 */ TSGrowableArray<T> m_array;
    /* 0010 */ uint32_t m_linkOffset;
};

class CSBasePriority {
    public:
    /* 0000 */ CSBasePriorityQueue<void>* m_queue;
    /* 0004 */ uint32_t m_index;
};

struct EvtHandler {
    /* 0000 */ TSLink<EvtHandler> m_link;
    /* 0008 */ int32_t (__stdcall* m_func )(const void*, void*);
    /* 000C */ void* m_param;
    /* 0010 */ float m_priority;
    /* 0014 */ int32_t m_marker;
};

struct EvtKeyDown {
    /* 0000 */ TSLink<EvtKeyDown> m_link;
    /* 0008 */ KEY m_key;
};

struct TExtraInstanceRecyclable {
    /* 0000 */ uint32_t m_recycleBytes;
};

struct EvtMessage : public TExtraInstanceRecyclable {
    /* 0004 */ TSLink<EvtMessage> m_link;
    /* 000C */ uint8_t m_id[4];
    /* 0010 */ char m_data [4];
};

class TSTimerPriority_uint32_t : public CSBasePriority {
    public:
    /* 000C */ uint32_t m_val;

    virtual ~TSTimerPriority_uint32_t();
};

struct EvtTimer {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ TSTimerPriority_uint32_t m_targetTime;
    /* 0014 */ float m_timeout;
    /* 0018 */ int32_t (__stdcall* m_handler )(const void*, void*);
    /* 001C */ void* m_param;
    /* 0020 */ int32_t (__stdcall* m_guidHandler )(const void*, uint64_t, void*);
    /* 0024 */ uint8_t unk_0024[4];
    /* 0028 */ uint64_t m_guidParam;
    /* 0030 */ void* m_guidParam2;
    /* 0034 */ uint8_t unk_0034[4];
};

template <class T>
struct TSPriorityQueue : public CSBasePriority {
};

struct EvtTimerQueue : public TSPriorityQueue<EvtTimer> {
};

template <class T>
struct EvtIdTable {
    /* 0000 */ TSGrowableArray<T> m_allocArray;
    /* 0010 */ TSGrowableArray<uint32_t> m_freeArray;
};

template <class T>
class TInstanceId : public TSLinkedNode<T> {
    public:
    /* 000C */ uint32_t m_id;

    virtual ~TInstanceId();
};

template <class T>
struct TSingletonInstanceId : public TInstanceId<T> {
};

class EvtContext : public TSingletonInstanceId<EvtContext> {
    public:
    /* 0010 */ SCritSect m_critsect;
    /* 0038 */ uint32_t m_currTime;
    /* 003C */ SCHEDSTATE m_schedState;
    /* 0040 */ TSTimerPriority_uint32_t m_schedNextWakeTime;
    /* 0050 */ uint32_t m_schedLastIdle;
    /* 0054 */ uint32_t m_schedFlags;
    /* 0058 */ uint32_t m_schedIdleTime;
    /* 005C */ uint32_t m_schedInitialIdleTime;
    /* 0060 */ uint32_t m_schedWeight;
    /* 0064 */ uint32_t m_schedSmoothWeight;
    /* 0068 */ int32_t m_schedRebalance;
    /* 006C */ TSExplicitList<EvtHandler> m_queueHandlerList [36];
    /* 021C */ TSExplicitList<EvtMessage> m_queueMessageList;
    /* 0228 */ uint32_t m_queueSyncButtonState;
    /* 022C */ TSExplicitList<EvtKeyDown> m_queueSyncKeyDownList;
    /* 0238 */ EvtIdTable<EvtTimer*> m_timerIdTable;
    /* 0258 */ EvtTimerQueue m_timerQueue;
    /* 0260 */ HPROPCONTEXT m_propContext;
    /* 0264 */ int32_t unk_0264;
    /* 0268 */ int32_t unk_0268;
    /* 026C */ int32_t unk_026C;
    /* 0270 */ void* m_callContext;
    /* 0274 */ uint32_t m_startWatchdog;
};

template <class T>
struct TInstanceIdTable {
    /* 0000 */ SCritSect m_idCritSect;
    /* 0028 */ uint32_t m_id;
    /* 002C */ int32_t m_idWrapped;
    /* 0030 */ CSRWLock m_idLock [8];
    /* 0090 */ TSList<T> m_idList [8];
};

#endif
