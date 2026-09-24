#ifndef ENGINE_SOURCE_BASE_STATUS_H
#define ENGINE_SOURCE_BASE_STATUS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"

class CStatus {
    public:
    struct STATUSENTRY {
        /* 0000 */ char* m_text;
        /* 0004 */ STATUS_TYPE m_severity;
        /* 0008 */ TSLink<CStatus::STATUSENTRY> m_link;

        ~STATUSENTRY();
    };

    /* 0004 */ TSExplicitList<CStatus::STATUSENTRY, offsetof(CStatus::STATUSENTRY, m_link)> m_statusList;
    /* 0010 */ STATUS_TYPE m_highestSeverity;

    virtual ~CStatus();
    virtual void Display();
    virtual void Add(const CStatus& source);
    virtual void Add(STATUS_TYPE severity, const char* format, ...);
    virtual void Prepend(STATUS_TYPE severity, const char* format, ...);

    CStatus();
    void Clear();
    char* GetErrorStr(char* buffer, size_t bufchars, STATUS_TYPE minSeverity, const char* separator);
    char* GetErrorStrAlloc(STATUS_TYPE minSeverity, const char* separator);
    uint32_t GetErrorStrLen(STATUS_TYPE minSeverity, const char* separator);
    int32_t IsEmpty();
};

CStatus& GetGlobalStatusObj();

// INLINED
inline CStatus::STATUSENTRY::~STATUSENTRY() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }
}

// INLINED
inline CStatus::CStatus() {
    this->m_highestSeverity = STATUS_INFO;
}

// INLINED
inline void CStatus::Clear() {
    this->m_statusList.Clear();
    this->m_highestSeverity = STATUS_INFO;
}

#endif
