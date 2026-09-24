#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREADLOCALSTORAGE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREADLOCALSTORAGE_H

#include <cstdint>

namespace Blizzard {
namespace Thread {

typedef void (*TLSDestructor)(void*);

struct TLSSlot {
    /* 0000 */ uint32_t m_key;
    /* 0004 */ TLSDestructor m_destructor;
};

void* RegisterLocalStorage(TLSSlot* slot, void* (*constructor)(void*), void* param, TLSDestructor destructor);

}
}

namespace System_Thread {

extern Blizzard::Thread::TLSSlot* s_slotList[128];
extern uint32_t s_slotListUsed;

bool AllocateLocalStorage(Blizzard::Thread::TLSSlot* slot, Blizzard::Thread::TLSDestructor destructor);
bool AllocateTLSSlot(Blizzard::Thread::TLSSlot* slot, Blizzard::Thread::TLSDestructor destructor);
void InternalSetLocalStorage(const Blizzard::Thread::TLSSlot* slot, const void* value);
void RunTLSDestructors();

}

#endif
