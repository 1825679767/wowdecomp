#include "SLock.h"
#include "SThread.h"
#include "Storm/H/storm.h"

// OFFSET: 0x00774620
SCritSect::SCritSect() {
    InitializeCriticalSection(&this->m_critsect);
}

// OFFSET: 0x00774630
SCritSect::~SCritSect() {
    DeleteCriticalSection(&this->m_critsect);
}

// OFFSET: 0x00774640
void SCritSect::Enter() {
    EnterCriticalSection(&this->m_critsect);
}

// OFFSET: 0x00774650
void SCritSect::Leave() {
    LeaveCriticalSection(&this->m_critsect);
}

// OFFSET: 0x00774690
uint32_t SSyncObject::Wait(uint32_t timeout) {
    return WaitForSingleObject(this->m_opaqueData, timeout);
}

// OFFSET: 0x007746B0
uint32_t SSyncObject::WaitMultiplePtr(uint32_t count, SSyncObject** objects, int32_t waitAll, uint32_t timeout) {
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];

    if (count > MAXIMUM_WAIT_OBJECTS) {
        return 0xFFFFFFFF;
    }

    uint32_t handleCount = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (objects[i] && objects[i]->m_opaqueData) {
            handles[handleCount++] = objects[i]->m_opaqueData;
        }
    }

    return WaitForMultipleObjects(handleCount, handles, waitAll, timeout);
}

// OFFSET: 0x00774720
int32_t SEvent::Set() {
    return SetEvent(this->m_opaqueData);
}

// OFFSET: 0x00774730
int32_t SEvent::Reset() {
    return ResetEvent(this->m_opaqueData);
}

// OFFSET: 0x00774770
int32_t SThread::SetPriority(int32_t priority) {
    static const int32_t priorities[] = { THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL };

    return priority < 6 && SetThreadPriority(this->m_opaqueData, priorities[priority]);
}

// OFFSET: 0x007747A0
int32_t SSemaphore::Signal(uint32_t count) {
    return ReleaseSemaphore(this->m_opaqueData, count, nullptr);
}

// OFFSET: 0x007747C0
void SMutex::Create(int32_t initialOwner, const char* name) {
    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = nullptr;
    }

    this->m_opaqueData = CreateMutexA(nullptr, initialOwner, name);

    if (!this->m_opaqueData) {
        DebugLog("SMutex::Create: CreateMutex failed");
    }
}

// OFFSET: 0x00774810
int32_t SMutex::Release() {
    return ReleaseMutex(this->m_opaqueData);
}

// OFFSET: 0x007748E0
SSyncObject::~SSyncObject() {
    if (this->m_opaqueData) {
        CloseHandle(this->m_opaqueData);
        this->m_opaqueData = nullptr;
    }
}

// OFFSET: 0x00774900
SEvent::SEvent(int32_t manualReset, int32_t initialState) {
    this->m_opaqueData = CreateEventA(nullptr, manualReset, initialState, nullptr);

    if (!this->m_opaqueData) {
        DebugLog("SEvent::SEvent: CreateEvent failed");
    }
}

// OFFSET: 0x00774940
SSemaphore::SSemaphore(int32_t initialCount, int32_t maximumCount) {
    this->m_opaqueData = CreateSemaphoreA(nullptr, initialCount, maximumCount, nullptr);
}

// OFFSET: 0x00774970
SMutex::SMutex() {
    this->Create(0, nullptr);
}
