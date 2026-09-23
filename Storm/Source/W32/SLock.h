#ifndef STORM_SOURCE_W32_SLOCK_H
#define STORM_SOURCE_W32_SLOCK_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class SCritSect {
    public:
    /* 0000 */ CRITICAL_SECTION m_critsect;
    /* 0018 */ uint8_t unk_0018[16];

    SCritSect();
    ~SCritSect();
    void Enter();
    void Leave();
};

class SSyncObject {
    public:
    static uint32_t WaitMultiplePtr(uint32_t count, SSyncObject** objects, int32_t waitAll, uint32_t timeout);

    /* 0000 */ HANDLE m_opaqueData;

    SSyncObject();
    ~SSyncObject();
    uint32_t Wait(uint32_t timeout);
};

class SEvent : public SSyncObject {
    public:
    SEvent(int32_t manualReset, int32_t initialState);
    int32_t Reset();
    int32_t Set();
};

class SMutex : public SSyncObject {
    public:
    SMutex();
    void Create(int32_t initialOwner, const char* name);
    int32_t Release();
};

class SSemaphore : public SSyncObject {
    public:
    SSemaphore(int32_t initialCount, int32_t maximumCount);
    int32_t Signal(uint32_t count);
};

// OFFSET: 0x00921A00
inline SSyncObject::SSyncObject() {
    this->m_opaqueData = nullptr;
}

#endif
