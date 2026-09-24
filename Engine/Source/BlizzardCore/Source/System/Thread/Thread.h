#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREAD_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_THREAD_THREAD_H

#include <cstdint>
#include <functional>
#include <set>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

namespace System_Thread {

struct ThreadWorker;

}

namespace Blizzard {
namespace Thread {

struct ThreadPoolRecord;

struct ThreadRecord {
    /* 0000 */ HANDLE m_handle;
    /* 0004 */ uint32_t m_threadId;
    /* 0008 */ void* m_param;
    /* 000C */ uint32_t (*m_function)(void*);
    /* 0010 */ volatile int32_t m_state;
    /* 0014 */ Blizzard::Lock::Atomic m_refCount;
    /* 0018 */ uint32_t m_result;
    /* 001C */ System_Thread::ThreadWorker* m_worker;
    /* 0020 */ uint8_t unk_0020[16];
    /* 0030 */ char m_name[8];
};

bool Close(ThreadRecord* thread);
bool Create(uint32_t (*function)(void*), void* param, ThreadRecord** thread, const char* name, int32_t flags, int32_t stackSize);
bool CreateDetached(uint32_t (*function)(void*), void* param, const char* name);

}
}

namespace System_Thread {

struct ThreadWorker {
    /* 0000 */ Blizzard::Thread::ThreadRecord* m_thread;
    /* 0004 */ Blizzard::Thread::ThreadPoolRecord* m_pool;
    /* 0008 */ Blizzard::Lock::Event m_wakeEvent;
    /* 000C */ Blizzard::Lock::Event m_readyEvent;
    /* 0010 */ Blizzard::Thread::ThreadRecord* m_task;
    /* 0014 */ uint32_t (*m_taskFunction)(void*);
    /* 0018 */ void* m_taskParam;
    /* 001C */ int32_t m_priority;
    /* 0020 */ int32_t m_affinity;
};

struct ExternalTLSRecord {
    /* 0000 */ HANDLE m_thread;
    /* 0004 */ HANDLE m_event;
    /* 0008 */ HANDLE m_watcher;
    /* 000C */ uint32_t m_threadId;
    /* 0010 */ void* m_value;
    /* 0014 */ Blizzard::Thread::TLSDestructor m_destructor;
};

extern bool s_initialized;
extern Blizzard::Lock::Mutex s_mutex;
extern Blizzard::Lock::Mutex s_registryMutex;
extern Blizzard::Thread::TLSSlot s_threadRecordTLS;
extern Blizzard::Thread::TLSSlot s_stackTraceEntryPointTLS;
extern Blizzard::Thread::ThreadRecord* s_mainThread;
extern std::set<Blizzard::Thread::ThreadRecord*, std::less<Blizzard::Thread::ThreadRecord*>, Blizzard::Memory::AllocSTL_raw<Blizzard::Thread::ThreadRecord*>>* s_ThreadRegistry;
extern int32_t s_DefaultStackSize;
extern ExternalTLSRecord s_externalTLS[32];
extern int32_t s_externalTLSCount;

void AddToRegistry(Blizzard::Thread::ThreadRecord* thread);
uint32_t ExternalTLSWatcher(void* param);
bool GetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t* level);
Blizzard::Thread::ThreadRecord* GetThreadRecord();
void InitMainThread();
void InitStackTraceEntryPoint();
void InitThreadSystem();
Blizzard::Thread::ThreadRecord* NewThread(uint32_t (*function)(void*), void* param, const char* name);
void RegisterExternalTLSDestructor(void* value, Blizzard::Thread::TLSDestructor destructor);
void RemoveFromRegistry(Blizzard::Thread::ThreadRecord* thread);
void RunExternalTLSDestructors(HANDLE thread);
bool SetPriorityLevel(Blizzard::Thread::ThreadRecord* thread, int32_t level);
void SetStackTraceEntryPoint(void* entryPoint);
void ShutdownThreadLocalStorage();
uint32_t __stdcall ThreadStarter(void* param);

}

#endif
