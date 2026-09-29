#ifndef _TRIAGE_CWOWCLIENTSTATUS_H
#define _TRIAGE_CWOWCLIENTSTATUS_H

#include <cstdint>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Storm/H/storm.h"

class CWOWClientStatus : public CStatus {
    public:
    /* 0014 */ HSLOG m_log;

    virtual ~CWOWClientStatus();

    CWOWClientStatus(const char* logFileName);
    CWOWClientStatus(const char* logFileName, int32_t append);
};

#endif
