#ifndef _TRIAGE_LOGINRESPONSE_H
#define _TRIAGE_LOGINRESPONSE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class LoginResponse {
    public:
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;

    virtual ~LoginResponse();
};

#endif
