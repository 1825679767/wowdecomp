#ifndef ENGINE_SOURCE_STORM_H_SCRITSECT_H
#define ENGINE_SOURCE_STORM_H_SCRITSECT_H

#include <cstdint>
#include <windows.h>

class CCritSect {
    public:
    /* 0000 */ CRITICAL_SECTION m_critsect;

    CCritSect();
    ~CCritSect();
    void Enter();
    void Enter(int32_t forwriting);
    void Leave();
    void Leave(int32_t fromwriting);
};

inline CCritSect::CCritSect() {
    InitializeCriticalSection(&this->m_critsect);
}

inline CCritSect::~CCritSect() {
    DeleteCriticalSection(&this->m_critsect);
}

inline void CCritSect::Enter() {
    EnterCriticalSection(&this->m_critsect);
}

inline void CCritSect::Enter(int32_t forwriting) {
    EnterCriticalSection(&this->m_critsect);
}

inline void CCritSect::Leave() {
    LeaveCriticalSection(&this->m_critsect);
}

inline void CCritSect::Leave(int32_t fromwriting) {
    LeaveCriticalSection(&this->m_critsect);
}

#endif
