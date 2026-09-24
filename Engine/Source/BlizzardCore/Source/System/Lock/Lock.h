#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_LOCK_LOCK_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_LOCK_LOCK_H

#include <cstdint>
#include <windows.h>

namespace Blizzard {
namespace Lock {

typedef HANDLE Event;
typedef CRITICAL_SECTION Mutex;

class Atomic {
    public:
    /* 0000 */ volatile int32_t m_value;

    int32_t Decrement();
    int32_t Increment();
};

class EventObject {
    public:
    /* 0000 */ Event m_event;

    EventObject();
    ~EventObject();
};

class RMutex {
    public:
    /* 0000 */ Mutex m_mutex;

    RMutex();
    ~RMutex();
};

class ScopedMutex {
    public:
    /* 0000 */ Mutex* m_mutex;

    ScopedMutex(Mutex& mutex);
    ~ScopedMutex();
};

void EventDestroy(Event& event);
void EventSet(Event& event);
Event* EventWait(uint32_t timeout, Event& event);
void EventWaitForever(Event& event);
void MutexCreate(Mutex& mutex);
void MutexDestroy(Mutex& mutex);
void MutexEnter(Mutex& mutex);
void MutexLeave(Mutex& mutex);

}
}

#endif
